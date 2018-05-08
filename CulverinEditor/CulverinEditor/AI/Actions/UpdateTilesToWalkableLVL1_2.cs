using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class UpdateTilesToWalkableLVL1_2 : CulverinBehaviour
{
    
    void Start()
    {
        GameObject map = GetLinkedObject("map_obj");
        LevelMap map_level = map.GetComponent<LevelMap>();
        if (map_level != null)
        {
            map_level.UpdateMap(26, 11, 0);
			map_level.UpdateMap(0, 12, 0);
        }
    }
  
}