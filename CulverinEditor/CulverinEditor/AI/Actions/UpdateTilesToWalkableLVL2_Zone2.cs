using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class UpdateTilesToWalkableLVL2_Zone2 : CulverinBehaviour
{
    
    void Start()
    {
        GameObject map = GetLinkedObject("map_obj");
        LevelMap map_level = map.GetComponent<LevelMap>();
        if (map_level != null)
        {
            map_level.UpdateMap(9, 10, 0);
			map_level.UpdateMap(12, 18, 0);
        }
    }
  
}