using CulverinEditor;

public class PuzzleLogicMap
{
    public int[,] walkability;
    // 0: No walkable
    // 1: Walkable
    public int width;
    public int height;

    public PuzzleLogicMap(int _width, int _height)
    {
        width = _width;
        height = _height;

        walkability = new int[width, height];

        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                walkability[x, y] = 0;
            }
        }
    }
}

public class PuzzlePath1 : CulverinBehaviour
{
    public PuzzleLogicMap logic_map;

    public int width = 6;
    public int height = 6;

    // -------------------------

    void Start()
    {
        logic_map = new PuzzleLogicMap(width, height);

        logic_map.walkability[1, 0] = 1;
        logic_map.walkability[1, 1] = 1;
        logic_map.walkability[2, 1] = 1;
        logic_map.walkability[2, 2] = 1;
        logic_map.walkability[3, 2] = 1;
        logic_map.walkability[3, 3] = 1;
        logic_map.walkability[3, 4] = 1;
        logic_map.walkability[4, 4] = 1;
        logic_map.walkability[4, 5] = 1;

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