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

        // 0: No walkable
        // 1: Walkable
        logic_map.walkability[2, 5] = 1;
        logic_map.walkability[2, 4] = 1;
        logic_map.walkability[3, 4] = 1;
        logic_map.walkability[3, 3] = 1;
        logic_map.walkability[3, 2] = 1;
        logic_map.walkability[2, 2] = 1;
        logic_map.walkability[1, 2] = 1;
        logic_map.walkability[1, 1] = 1;
        logic_map.walkability[1, 0] = 1;

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