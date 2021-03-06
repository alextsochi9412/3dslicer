import slicer, vtk, qt
from RFViewerHomeLib import removeNodeFromMRMLScene, Signal


class RFCanal:
  def __init__(self, radius):
    self.modelNode = None
    self.markupNode = None
    self.markupNodeObservers = []
    self.radius = radius
    self.markupPointsModifiedSignal = Signal("RFCanal")

  def __del__(self):
    self.deleteFromScene()

  def isPresentInScene(self):
    return self.markupNode is not None

  def deleteFromScene(self):
    if not self.isPresentInScene():
      return

    for observers in self.markupNodeObservers:
      self.markupNode.RemoveObserver(observers)

    removeNodeFromMRMLScene(self.modelNode)
    removeNodeFromMRMLScene(self.markupNode)
    self.modelNode = None
    self.markupNode = None
    self.markupNodeObservers.clear()

  def isVisible(self):
    return self.markupNode.GetDisplayNode().GetVisibility() if self.isPresentInScene() else False

  def generateCanal(self):
    self.deleteFromScene()
    self._createModel()
    self._createMarkupDisplayNode()

  def _createModel(self):
    self.modelNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLModelNode')
    self.modelNode.SetUndoEnabled(True)
    self.modelNode.SetName(slicer.mrmlScene.GetUniqueNameByString('CanalModel'))

    modelDisplayNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLModelDisplayNode')
    modelDisplayNode.BackfaceCullingOff()
    modelDisplayNode.Visibility2DOn()
    modelDisplayNode.SetSliceIntersectionThickness(2)
    modelDisplayNode.SetOpacity(0.3)  # Between 0-1, 1 being opaque
    
    settings = qt.QSettings()
    col = qt.QColor(settings.value("MarkupColorHex", "#ff8080"))
    modelDisplayNode.SetColor(float(col.red())/255.0, float(col.green())/255.0, float(col.blue())/255.0)

    self.modelNode.SetAndObserveDisplayNodeID(modelDisplayNode.GetID())
    self.modelNode.GetDisplayNode().Visibility2DOn()

  def _createMarkupDisplayNode(self):
    displayNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLMarkupsDisplayNode')
    displayNode.SetTextScale(0)
    self.markupNode = slicer.mrmlScene.AddNewNodeByClass('vtkMRMLMarkupsFiducialNode')
    self.markupNode.SetUndoEnabled(True)
    self.markupNode.SetName(slicer.mrmlScene.GetUniqueNameByString('カナル'))
    # self.markupNode.SetAndObserveDisplayNodeID(displayNode.GetID())
    self.markupNode.GetDisplayNode().SetTextScale(0)
    settings = qt.QSettings()
    # col = qt.QColor(settings.value("MarkupColorHex", "#379ede"))
    col = qt.QColor(settings.value("MarkupColorHex", "#ff8080"))
    # SetColor doesn't affect to color in the canal panel
    self.markupNode.GetDisplayNode().SetSelectedColor(float(col.red())/255.0, float(col.green())/255.0, float(col.blue())/255.0)
    self._observeMarkupNode()

  def _observeMarkupNode(self):
    def emitCanalModified(*args):
      self.markupPointsModifiedSignal.emit(self)

    # Set and observe new parameter node
    eventIds = [vtk.vtkCommand.ModifiedEvent,
      slicer.vtkMRMLMarkupsNode.PointModifiedEvent,
      slicer.vtkMRMLMarkupsNode.PointAddedEvent,
      slicer.vtkMRMLMarkupsNode.PointRemovedEvent]

    for eventId in eventIds:
      self.markupNodeObservers.append(self.markupNode.AddObserver(eventId, emitCanalModified))

    self.markupNodeObservers.append(
      self.markupNode.AddObserver(slicer.vtkMRMLMarkupsNode.DisplayModifiedEvent, self._onMarkupDisplayChanged))

  def _onMarkupDisplayChanged(self, *args):
    if not self.isPresentInScene():
      return

    self.modelNode.GetDisplayNode().SetVisibility(self.isVisible())

  def getID(self):
    return self.markupNode.GetID() if self.isPresentInScene() else ""

  def getParameterDict(self):
    return {  #
      "markupID": self.markupNode.GetID(),  #
      "modelID": self.modelNode.GetID(),  #
      "radius": str(self.radius)  #
    } if self.isPresentInScene() else {}

  @staticmethod
  def createFromParameterDict(parameterDict):
    """Reconstruct canal from scene using parameter IDs stored in the input dictionary"""
    canal = RFCanal(0)

    canal.radius = float(parameterDict["radius"])
    markupID = parameterDict["markupID"]
    modelId = parameterDict["modelID"]
    canal.markupNode = slicer.mrmlScene.GetNodeByID(markupID)
    canal.modelNode = slicer.mrmlScene.GetNodeByID(modelId)
    if None in [canal.markupNode, canal.modelNode]:
      raise ValueError(f"Canal markup and models are not both present in the scene {markupID}, {modelId}")

    canal._observeMarkupNode()
    return canal
