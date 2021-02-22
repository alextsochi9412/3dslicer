/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

#ifndef __qSlicerSubjectHierarchyMarkupsPlugin_h
#define __qSlicerSubjectHierarchyMarkupsPlugin_h

// SubjectHierarchy includes
#include "qSlicerSubjectHierarchyAbstractPlugin.h"

#include "qSlicerMarkupsSubjectHierarchyPluginsExport.h"

class qSlicerSubjectHierarchyMarkupsPluginPrivate;

/// \ingroup Slicer_QtModules_SubjectHierarchy_Plugins
class Q_SLICER_MARKUPS_SUBJECT_HIERARCHY_PLUGINS_EXPORT qSlicerSubjectHierarchyMarkupsPlugin : public qSlicerSubjectHierarchyAbstractPlugin
{
public:
  Q_OBJECT

public:
  typedef qSlicerSubjectHierarchyAbstractPlugin Superclass;
  qSlicerSubjectHierarchyMarkupsPlugin(QObject* parent = nullptr);
  ~qSlicerSubjectHierarchyMarkupsPlugin() override;

public:
  /// Determines if a data node can be placed in the hierarchy using the actual plugin,
  /// and gets a confidence value for a certain MRML node (usually the type and possibly attributes are checked).
  /// \param node Node to be added to the hierarchy
  /// \param parentItemID Prospective parent of the node to add.
  ///   Default value is invalid. In that case the parent will be ignored, the confidence numbers are got based on the to-be child node alone.
  /// \return Floating point confidence number between 0 and 1, where 0 means that the plugin cannot handle the
  ///   node, and 1 means that the plugin is the only one that can handle the node (by node type or identifier attribute)
  double canAddNodeToSubjectHierarchy(
    vtkMRMLNode* node,
    vtkIdType parentItemID=vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID )const override;

  /// Determines if the actual plugin can handle a subject hierarchy item. The plugin with
  /// the highest confidence number will "own" the item in the subject hierarchy (set icon, tooltip,
  /// set context menu etc.)
  /// \param item Item to handle in the subject hierarchy tree
  /// \return Floating point confidence number between 0 and 1, where 0 means that the plugin cannot handle the
  ///   item, and 1 means that the plugin is the only one that can handle the item (by node type or identifier attribute)
  double canOwnSubjectHierarchyItem(vtkIdType itemID)const override;

  /// Get role that the plugin assigns to the subject hierarchy item.
  ///   Each plugin should provide only one role.
  Q_INVOKABLE const QString roleForPlugin()const override;

  /// Get icon of an owned subject hierarchy item
  /// \return Icon to set, empty icon if nothing to set
  QIcon icon(vtkIdType itemID) override;

  /// Get visibility icon for a visibility state
  QIcon visibilityIcon(int visible) override;

  /// Set display color of an owned subject hierarchy item
  /// \param color Display color to set
  /// \param terminologyMetaData Map containing terminology meta data. Contents:
  ///   qSlicerTerminologyItemDelegate::TerminologyRole        : string
  ///   qSlicerTerminologyItemDelegate::NameRole               : string
  ///   qSlicerTerminologyItemDelegate::NameAutoGeneratedRole  : bool
  ///   qSlicerTerminologyItemDelegate::ColorAutoGeneratedRole : bool
  void setDisplayColor(vtkIdType itemID, QColor color, QMap<int, QVariant> terminologyMetaData) override;

  /// Get display color of an owned subject hierarchy item
  /// \param terminologyMetaData Output map containing terminology meta data. Contents:
  ///   qSlicerTerminologyItemDelegate::TerminologyRole        : string
  ///   qSlicerTerminologyItemDelegate::NameRole               : string
  ///   qSlicerTerminologyItemDelegate::NameAutoGeneratedRole  : bool
  ///   qSlicerTerminologyItemDelegate::ColorAutoGeneratedRole : bool
  QColor getDisplayColor(vtkIdType itemID, QMap<int, QVariant> &terminologyMetaData)const override;

  /// Get view context menu item actions that are available when right-clicking an object in the views.
  /// These item context menu actions can be shown in the implementations of \sa showViewContextMenuActionsForItem
  QList<QAction*> viewContextMenuActions()const override;

  /// Show context menu actions valid for a given subject hierarchy item to be shown in the view.
  /// \param itemID Subject Hierarchy item to show the context menu items for
  /// \param eventData Supplementary data for the item that may be considered for the menu (sub-item ID, attribute, etc.)
  void showViewContextMenuActionsForItem(vtkIdType itemID, QVariantMap eventData) override;

protected slots:
  /// Called when clicking on rename point action
  void renamePoint();
  /// Called when clicking on delete point action
  void deletePoint();
  /// Called when clicking on toggle select point action
  void toggleSelectPoint();
  /// Called when clicking on handle interactive action
  void toggleHandleInteractive();

protected:
  QScopedPointer<qSlicerSubjectHierarchyMarkupsPluginPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerSubjectHierarchyMarkupsPlugin);
  Q_DISABLE_COPY(qSlicerSubjectHierarchyMarkupsPlugin);
};

#endif