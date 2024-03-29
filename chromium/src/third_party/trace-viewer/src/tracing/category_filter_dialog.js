// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

'use strict';

/**
 * @fileoverview CategoryFilterButton extracts categories from the model
 * and shows/hides them based on settings.
 */
base.require('tracing.filter');
base.require('ui.overlay');
base.exportTo('tracing', function() {

  var CategoryFilterDialog = ui.define('div');

  CategoryFilterDialog.prototype = {
    __proto__: ui.Overlay.prototype,

    decorate: function() {
      ui.Overlay.prototype.decorate.call(this);

      this.className = 'view-category-filter-overlay';
      this.autoClose = true;

      var containerEl = document.createElement('div');
      containerEl.className = 'category-filter-dialog';
      containerEl.textContent = 'Select active categories:';

      this.formEl_ = document.createElement('form');
      this.formEl_.className = 'category-filter-dialog-form';
      containerEl.appendChild(this.formEl_);
      this.appendChild(containerEl);

      this.categoriesEl_ = document.createElement('div');
      this.categoriesEl_.className = 'categories';
      this.formEl_.appendChild(this.categoriesEl_);

      this.addEventListener('visibleChange', this.onVisibleChange_.bind(this));
    },

    set categories(c) {
      this.categories_ = c;
    },

    set settingUpdatedCallback(c) {
      this.settingUpdatedCallback_ = c;
    },

    unselectedCategories_: function() {
      var inputs = this.formEl_.querySelectorAll('input');
      var categories = [];
      for (var i = 0; i < inputs.length; ++i) {
        var input = inputs[i];
        if (input.checked === false)
          categories.push(input.value);
      }
      return categories;
    },

    onVisibleChange_: function() {
      if (this.visible) {
        this.updateForm_();
      }
    },

    updateForm_: function() {
      this.categoriesEl_.innerHTML = ''; // Clear old categories

      var categories = this.categories_.sort();
      for (var i = 0; i < categories.length; i++) {
        var category = categories[i];
        var inputEl = document.createElement('input');
        inputEl.type = 'checkbox';
        inputEl.id = category;
        inputEl.value = category;

        inputEl.checked = true;
        inputEl.onchange = this.updateSetting_.bind(this);

        var labelEl = document.createElement('label');
        labelEl.textContent = category;
        labelEl.setAttribute('for', category);

        var divEl = document.createElement('div');
        divEl.appendChild(inputEl);
        divEl.appendChild(labelEl);
        this.categoriesEl_.appendChild(divEl);
      }
    },

    updateSetting_: function(e) {
      var checkbox = e.target;
      if (this.settingUpdatedCallback_ !== undefined)
        this.settingUpdatedCallback_(this.unselectedCategories_());
    }
  };

  return {
    CategoryFilterDialog: CategoryFilterDialog
  };
});
