using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class UpdateTilesToWalkableLVL1_3 : CulverinBehaviour
{
    
    void Start()
    {
        GameObject map = GetLinkedObject("map_obj");
        LevelMap map_level = map.GetComponent<LevelMap>();
        if (map_level != null)
        {
            map_level.UpdateMap(13, 7, 0);
        }
    }
  
}