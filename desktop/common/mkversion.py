#!/usr/bin/env python
"""Product version related utlities"""

import sys
import json
import os.path

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
MAC_BUNDLE_ID_TEMPLATE = "com.operasoftware.Opera%s"

def get_repo_root(path):
  """Get the absolute path to the repository of the current script."""
  root_dir = os.path.dirname(path)
  while (root_dir != os.path.dirname(root_dir) and
         not os.path.exists(os.path.join(root_dir, "chromium"))):
    root_dir = os.path.dirname(root_dir)
  return root_dir

class VersionUtils:
  """Utility class to generate build_version.h, package name etc. from the
  two version files: 1. product version having the major.minor data 2. common
  version having the nightly.patch(.build) data"""

  def __init__(self, repo_root = "", output_file = "build_version.h", debug = False):
    # prv_file = 'desktop/common/VERSION', comv_file = 'common/VERSION'):
    self._prv_fpath = os.path.abspath(os.path.join(repo_root,
                                                   'desktop', 'common', 'VERSION'))
    self._brand_fpath = os.path.abspath(os.path.join(repo_root,
                                                   'desktop', 'common', 'BRANDING'))
    self._comv_fpath = os.path.abspath(os.path.join(repo_root,
                                                    'common', 'VERSION'))
    self._ubn_fpath = os.path.abspath(os.path.join(repo_root,
                                                   'common', 'UBN'))
    self._bh_fpath = output_file
    self._debug = debug

  def _ReadVersionData(self):
    """Reads the version files and returns a dict"""
    version_data = {}
    for fpath in (self._prv_fpath, self._comv_fpath, self._brand_fpath):
        try:
            vfile = open(fpath)
        except IOError:
            raise BaseException("Missing file: %s" % fpath)

        vdata = json.loads(vfile.read())
        vfile.close()
        for k in vdata:
            version_data[k] = vdata[k]
    # UBN file contains just the number of build
    try:
      vfile = open(self._ubn_fpath, 'r')
      version_data['ubn'] = vfile.readline().strip()
    except IOError:
      # UBN file can be missing - it is only generated on buildbot servers
      pass
    return version_data

  def GetVersionInfo(self):
    """Outputs version related data"""

    def _Sanitise(data, expected, maxint = 65535):
        """Tiny checks on the input from VERSION file"""
        out = None
        try:
            out = expected(data)
        except Exception:
            sys.exit('Threw exception while converting input from VERSION file')

        if expected == int:
            if out < 0 or out > maxint:
                sys.exit("Expected a number between 0 and %d, got %d" % (maxint, out))
        elif expected == str:
            import re
            out = re.sub(r'[^a-zA-Z0-9_.]+', '', out)
        return out

    version_data = self._ReadVersionData()
    if self._debug:
        print 'Read:' + repr(version_data)
    major = minor = nightly = patch = build = 0
    name = product_name = user_agent_name = "unknown"
    ubn = -1  # 0 is valid unique build number, -1 means no ubn
    sanitise = _Sanitise
    for k in version_data:
        if k == 'major':
            major = sanitise(version_data['major'], int)
        elif k == 'minor':
            minor = sanitise(version_data['minor'], int)
        elif k == 'nightly':
            nightly = sanitise(version_data['nightly'], int)
        elif k == 'patch':
            patch = sanitise(version_data['patch'], int)
        elif k == 'build':
            build = sanitise(version_data['build'], int)
        elif k == 'name':
            name = sanitise(version_data['name'], str).capitalize()
        elif k == 'product_name':
            product_name = sanitise(version_data['product_name'], str)
        elif k == 'user_agent_name':
            user_agent_name = sanitise(version_data['user_agent_name'], str)
        elif k == 'ubn':
            ubn = sanitise(version_data['ubn'], int, sys.maxint)

    if self._debug:
        print 'Version data:', major, minor, nightly, patch, build, name

    # This has to be in sync with the GetVersionInfoDict() method below!
    return (major, minor, nightly, patch, build, ubn, name, product_name, user_agent_name)

  def GetVersionInfoDict(self):
    major, minor, nightly, patch, build, ubn, name, product_name, user_agent_name = self.GetVersionInfo()
    ret_dict = {
      'major': major,
      'minor': minor,
      'nightly': nightly,
      'patch': patch,
      'build': build,
      'ubn': ubn,
      'name': name,
      'product_name': product_name,
      'user_agent_name': user_agent_name
    }
    return ret_dict

  def MakeHeader(self):
    """Writes the header file with version info."""
    from datetime import date
    major, minor, nightly, patch, build, ubn, name, product_name, user_agent_name = self.GetVersionInfo()
    current_year = date.today().year
    bundle_id_suffix = ""
    if name.lower() in ["developer","next"]:
      bundle_id_suffix = name.capitalize()
    bundle_id = MAC_BUNDLE_ID_TEMPLATE % bundle_id_suffix

    windows_icon_rc = "../windows/classic/resources/opera_generated_res.rc"
    hfile = open(windows_icon_rc, 'w')
    hfile.write("""// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) """ + str(current_year) + """ Opera Software ASA.  All rights reserved.

// This file is generated by mkversion.py
// DO NOT MODIFY THIS FILE
""")
    hfile_product_name = "Opera Internet Browser"
    if name == 'Developer':
      hfile.write('OPERA   ICON   "desktop/windows/classic/resources/icons/opera_dev.ico"\n')
      hfile_product_name = "Opera Developer"
    elif name == 'Next':
      hfile.write('OPERA   ICON   "desktop/windows/classic/resources/icons/opera_next.ico"\n')
      hfile_product_name = "Opera Next"
    elif name == 'Stable':
      hfile.write('OPERA   ICON   "desktop/windows/classic/resources/icons/opera.ico"\n')
    else:
      sys.exit("Illegal channel name: %s" % name)

    hfile.write('\nVS_VERSION_INFO VERSIONINFO\n')
    hfile.write('  FILEVERSION {0},{1},{2},{3}\n'.format(major,minor,nightly,patch))
    hfile.write('  PRODUCTVERSION {0},{1},{2},{3}\n'.format(major,minor,nightly,patch))
    hfile.write("""  FILEFLAGSMASK VS_FFI_FILEFLAGSMASK
#ifdef _DEBUG
  FILEFLAGS VS_FF_DEBUG
#else
  FILEFLAGS 0x0L
#endif
  FILEOS VOS_NT_WINDOWS32
  FILETYPE VFT_APP
  FILESUBTYPE VFT2_UNKNOWN
BEGIN
  BLOCK "StringFileInfo"
  BEGIN
    BLOCK "040904b0"
    BEGIN
      VALUE "CompanyName", "Opera Software"
      VALUE "FileDescription", \"""" + hfile_product_name + """"
""")
    hfile.write('      VALUE "FileVersion", "{0}.{1}.{2}.{3}"\n'.format(major,minor,nightly,patch))
    hfile.write("""      VALUE "InternalName", "Opera"
      VALUE "LegalCopyright", "Copyright \\251 Opera Software """ + str(current_year) + """"
      VALUE "OriginalFilename", "launcher.exe"
      VALUE "ProductName", \"""" + hfile_product_name + """"
""")
    hfile.write('      VALUE "ProductVersion", "{0}.{1}.{2}.{3}"\n'.format(major,minor,nightly,patch))
    hfile.write("""    END
  END
  BLOCK "VarFileInfo"
  BEGIN
    VALUE "Translation", 0x409, 0x04B0
  END
END
""")
    hfile.close()

    dir_name = os.path.dirname(self._bh_fpath)
    if not os.path.exists(dir_name):
      os.makedirs(dir_name)
    hfile = open(self._bh_fpath, 'w')
    hfile.write("""// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) """ + str(current_year) + """ Opera Software ASA.  All rights reserved.

// This file is generated by mkversion.py
// DO NOT MODIFY THIS FILE

#ifndef DESKTOP_COMMON_BUILD_VERSION_H_
#define DESKTOP_COMMON_BUILD_VERSION_H_

namespace opera {

""")
    hfile.write('const char kProductName[] = "{0}";\n'.format(product_name))
    hfile.write('const char kBundleId[] = "{0}";\n'.format(bundle_id))
    hfile.write('const char kUserAgentProductID[] = "{0}";\n'.format(user_agent_name))
    hfile.write('const int kVersionMajor = {0};\n'.format(major))
    hfile.write('const int kVersionMinor = {0};\n'.format(minor))
    hfile.write('const int kVersionNightly = {0};\n'.format(nightly))
    hfile.write('const int kVersionPatch = {0};\n'.format(patch))
    hfile.write('const int kVersionBuild  = {0};\n'.format(build))
    hfile.write('const int kVersionYear = {0};\n'.format(current_year))
    hfile.write('const char kVersionName[] = "' + name + '";\n')
    if build: #is present and non-zero
        hfile.write('const char kVersionString[] = "{0}.{1}.{2}.{3}.{4}";\n'
                 .format(major,minor,nightly,patch,build))
    else:
        hfile.write('const char kVersionString[] = "{0}.{1}.{2}.{3}";\n'
                 .format(major,minor,nightly,patch,build))
    if build: #is present and non-zero
        hfile.write('const char kInternalVersionString[] = "{0}.{1}.{2}";\n'
                 .format(nightly,patch,build))
    else:
        hfile.write('const char kInternalVersionString[] = "{0}.{1}";\n'
                 .format(nightly,patch))
    hfile.write('const int kInternalBuildNumber = {0};\n'.format(ubn))
    hfile.write("\n}\n#endif  //DESKTOP_COMMON_BUILD_VERSION_H_\n")
    hfile.close()
    print "Created %s from VERSION file." % self._bh_fpath

  def GetPackageStr(self):
    """ Generates a package name from version files"""
    vinfo = self.GetVersionInfo()
    out = ""
    for item in vinfo:
        out += (str(item) + '-')
    out = out[:-1]
    return out

  def MacBundleID(self):
    (major, minor, nightly, patch, build, ubn, name, product_name, user_agent_name) = self.GetVersionInfo()
    if not name.lower() in [ "developer", "next" ]:
        name = ""
    return MAC_BUNDLE_ID_TEMPLATE % name.capitalize()

  def MacVersion(self):
    (major, minor, nightly, patch, build, ubn, name, product_name, user_agent_name) = self.GetVersionInfo()
    return "{0}.{1}.{2}.{3}".format(major, minor, nightly, patch)

  def MacFullVersion(self):
    (major, minor, nightly, patch, build, ubn, name, product_name, user_agent_name) = self.GetVersionInfo()
    build_str = ""
    if build:
      build_str = ".%d" % build
    return "{0}.{1}.{2}.{3}{4}".format(major, minor, nightly, patch, build_str)

  def MacShortVersion(self):
    (major, minor, nightly, patch, build, ubn, name, product_name, user_agent_name) = self.GetVersionInfo()
    return "{0}.{1}".format(major, minor)

  def MacDyLibVersion(self):
    (major, minor, nightly, patch, build, ubn, name, product_name, user_agent_name) = self.GetVersionInfo()
    return "{0}.{1}.{2}".format(major, nightly / 256, nightly % 256)

  def MacAppIconName(self):
    (major, minor, nightly, patch, build, ubn, name, product_name, user_agent_name) = self.GetVersionInfo()
    if name and len(name) and name.lower() in [ "developer", "next" ]:
      return "app-%s" % name.lower()
    return "app"

def main(args = None):
  """Creates version header file and related data"""
  from optparse import OptionParser
  argparser = OptionParser(description = "Version related utilities")
  argparser.add_option('-g', '--root', default = get_repo_root(SCRIPT_DIR),
                       help = "Path to the repo root (product VERSION file "
                       "is in desktop/common and common one is in common/ directory).")
  argparser.add_option('-o', '--output', default = 'build_version.h',
                       help = "Path to the output file.")
  argparser.add_option('-d', '--debug', help = "Debug output",
                       action='store_true', default = False)
  argparser.add_option('-p', '--packagestr',
                       action='store_true', default = False,
                       help = "Output the package string")
  argparser.add_option('-I', '--mac-bundle-identifier',
                       action='store_true', default = False,
                       help = "Output the Mac bundle identifier.")
  argparser.add_option('-V', '--mac-version',
                       action='store_true', default = False,
                       help = "Output the Mac version string.")
  argparser.add_option('-F', '--mac-full-version',
                       action='store_true', default = False,
                       help = "Output the Mac full version string.")
  argparser.add_option('-S', '--mac-short-version',
                       action='store_true', default = False,
                       help = "Output the Mac short version string.")
  argparser.add_option('-D', '--mac-dylib-version',
                       action='store_true', default = False,
                       help = "Output the Mac dylib version string.")
  argparser.add_option('-A', '--mac-app-icon-name',
                       action='store_true', default = False,
                       help = "Output the Mac app icon name.")

  (args, opts) = argparser.parse_args()
  # create the build_version.h file from VERSION file
  vutils = VersionUtils(args.root, args.output, args.debug)
  # output package string to stdout
  if args.packagestr:
    print "Package string:" + vutils.GetPackageStr()
  elif args.mac_bundle_identifier:
    print vutils.MacBundleID()
  elif args.mac_version:
    print vutils.MacVersion()
  elif args.mac_full_version:
    print vutils.MacFullVersion()
  elif args.mac_short_version:
    print vutils.MacShortVersion()
  elif args.mac_dylib_version:
    print vutils.MacDyLibVersion()
  elif args.mac_app_icon_name:
    print vutils.MacAppIconName()
  else:
    vutils.MakeHeader()
  sys.exit(0)


if __name__ == "__main__":
  main()
