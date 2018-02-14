using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

//Attach this script to the tank parent object if you want to see it rotate
public class TestMovement : CulverinBehaviour
{
    private float movSpeed = 0.0f;
    Vector3 endPosition;
    int distanceToMove = 10;
    int[,] array2Da;
    int map_width = 0;
    int map_height = 0;

    void Start()
    {
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();
        array2Da = new int[map_width, map_height];
        for (int i = 0; i < map_width; i++)
        {
            for (int j = 0; j < map_height; j++)
            {
                array2Da[i, j] = 0;
            }
        }
        //array2Da[0,0] = 1;
        //Debug.Log(array2Da[0,0].ToString());
        endPosition = GetComponent<Transform>().Position;
        string map = Map.GetMapString();
        Debug.Log(map);
        int t = 0;
        for (int i = 0; i < map_width; i++)
        {
            for (int j = 0; j < map_height; j++)
            {
                array2Da[i, j] = int.Parse(map[t].ToString());
                t += 1;
            }
        }
    }

    void Update()
    {
        GetComponent<Transform>().Position = Vector3.MoveTowards(GetComponent<Transform>().Position, endPosition, movSpeed * Time.DeltaTime());
        if (GetComponent<Transform>().Position == endPosition)
        {
            if (Input.KeyDown("A")) //Left
            {
                endPosition = new Vector3(endPosition.x + distanceToMove, endPosition.y, endPosition.z);
            }
            else if (Input.KeyDown("D")) //Right
            {
                endPosition = new Vector3(endPosition.x - distanceToMove, endPosition.y, endPosition.z);
            }
            else if (Input.KeyDown("W")) //Up
            {
                endPosition = new Vector3(endPosition.x, endPosition.y, endPosition.z + distanceToMove);
            }
            else if (Input.KeyDown("S")) //Down
            {
                endPosition = new Vector3(endPosition.x, endPosition.y, endPosition.z - distanceToMove);
            }
        }
    }
}