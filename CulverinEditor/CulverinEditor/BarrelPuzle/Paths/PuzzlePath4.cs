using CulverinEditor;

public class PuzzlePath4 : CulverinBehaviour
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
        logic_map.walkability[4, 5] = 0;
        logic_map.walkability[4, 4] = 0;
        logic_map.walkability[3, 4] = 0;
        logic_map.walkability[2, 4] = 0;
        logic_map.walkability[1, 4] = 0;
        logic_map.walkability[1, 3] = 0;
        logic_map.walkability[1, 2] = 0;
        logic_map.walkability[2, 2] = 0;
        logic_map.walkability[3, 2] = 0;
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