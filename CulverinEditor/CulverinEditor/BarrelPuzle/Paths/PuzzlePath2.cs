using CulverinEditor;

public class PuzzlePath2 : CulverinBehaviour
{
    public PuzzleLogicMap logic_map;

    public int width = 6;
    public int height = 6;

    // -------------------------

    void Start()
    {
        logic_map = new PuzzleLogicMap(width, height);

        // 0: Walkable
        // 1: No walkable
        logic_map.walkability[3, 5] = 0;
        logic_map.walkability[3, 4] = 0;  
        logic_map.walkability[3, 3] = 0;
        logic_map.walkability[2, 3] = 0;
        logic_map.walkability[2, 2] = 0;
        logic_map.walkability[2, 1] = 0;
        logic_map.walkability[3, 1] = 0;
        logic_map.walkability[3, 0] = 0;

        /*  Important: Build the path logic map like this
         * 
         *                                   ^
         *                                   |
         *                                   |
         *                                   |
         *                                   |
         *                                   |
         *                                   |
         *                                   |
         *                                   |
         * <---------------------------------|
         */
    }

}