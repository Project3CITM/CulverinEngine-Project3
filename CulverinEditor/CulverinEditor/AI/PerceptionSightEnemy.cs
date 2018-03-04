using CulverinEditor;
using CulverinEditor.Debug;

public class PerceptionSightEnemy : CulverinBehaviour
{
    public int frustum_lenght = 3;
    public int frustum_size = 1;
    public int tile_size = 2;
    //public GameObject player;

    void Start()
    {
        Debug.Log("PerceptionSightEnemy Start");
        //player = GetLinkedObject("player");
    }

    void Update()
    {
        //TODO: Optimization --> Check if player is out of range
        if (true)
        {
            // Debug.Log("Player X: " + player.GetComponent<MovementController>().curr_x);
            // Debug.Log("Player Y: " + player.GetComponent<MovementController>().curr_y);
            Debug.Log("Current Tile X: " + GetCurrentTileX());
            Debug.Log("Current Tile Y: " + GetCurrentTileY());
            Debug.Log("Frustum size: " + frustum_size);
            Debug.Log("Frustum lenght: " + frustum_lenght);
            bool player_seen = false;
            //TODO: Directions --> switch()
            //NORTH:
            for (int j = 0; j < frustum_lenght; j++)
            {
                Debug.Log("j: " + j);
                Debug.Log("Checking Tile Y: " + (GetCurrentTileY() - j));
                if (2 == (GetCurrentTileY() - j))//player.GetComponent<MovementController>().curr_y
                {
                    for (int i = -j; i <= j + (frustum_size - 1); i++)
                    {
                        Debug.Log("i: " + i);
                        Debug.Log("Checking Tile X: " + (GetCurrentTileX() + i - ((frustum_size - 1) / 2)));
                        if (2 == (GetCurrentTileX() + i - ((frustum_size - 1) / 2)))//player.GetComponent<MovementController>().curr_x
                        {
                            Debug.Log("Player Is INSIDE Vision");
                            player_seen = true;
                            break;
                        }
                    }
                }
                if (player_seen == true)
                    break;
            }

            if (player_seen == false)
                Debug.Log("Player Is OUTSIDE Vision");
        }
    }

    public int GetCurrentTileX()
    {
        return (int)((float)GetComponent<Transform>().local_position.x / tile_size);
    }

    public int GetCurrentTileY()
    {
        return (int)((float)GetComponent<Transform>().local_position.z / tile_size);
    }
}