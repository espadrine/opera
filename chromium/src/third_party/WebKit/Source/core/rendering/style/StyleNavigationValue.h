/*
 * Copyright (C) 2011 Kyounga Ra (kyounga.ra@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef StyleNavigationValue_h
#define StyleNavigationValue_h

namespace WebCore {

class StyleNavigationValue {
friend class RenderStyle;
public:
    StyleNavigationValue()
    :m_id(AtomicString("auto"))
    , m_target(AtomicString("current")) { }

    StyleNavigationValue(const char* id, const char* target)
    :m_id(AtomicString(id))
    , m_target(AtomicString(target)) { }

    StyleNavigationValue(const AtomicString& id, const AtomicString& target = AtomicString("current"))
    :m_id(id)
    , m_target(target)
    { }

    bool isAuto() const
    {
        return m_id == "auto";
    }

    bool operator==(const StyleNavigationValue& o) const
    {
        return m_id == o.m_id && m_target == o.m_target;
    }

    bool operator!=(const StyleNavigationValue& o) const
    {
        return !(*this == o);
    }

    void operator=(const StyleNavigationValue& o)
    {
        m_id = o.m_id;
        m_target = o.m_target;
    }

    const AtomicString& id() const { return m_id; }
    const AtomicString& target() const { return m_target; }

protected:
    AtomicString m_id;
    AtomicString m_target;
};

} // namespace WebCore

#endif // StyleNavigationValue_h
