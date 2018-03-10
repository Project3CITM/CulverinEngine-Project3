using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

//Attach this script to the tank parent object if you want to see it rotate
public class MovementController : CulverinBehaviour
{
    public enum Direction
    {
        NORTH = 0,
        EAST,
        SOUTH,
        WEST
    }
    public enum Facing
    {
        UP = 0,
        STRAIGHT,
        DOWN
    }

    public GameObject enemy_obj;

    public Direction curr_dir = Direction.NORTH;
    public Facing curr_fac = Facing.STRAIGHT;
    public int start_direction = 1;
    public float movSpeed = 1.0f;
    Vector3 endPosition;
    Vector3 endRotation;
    public float distanceToMove = 10.0f;
    int[,] array2Da;
    public int curr_x = 0;
    public int curr_y = 0;
    private int tile_mov_x = 0;
    private int tile_mov_y = 0;
    int map_width = 0;
    int map_height = 0;
    public bool blocked_camera = false;

    public float up_angle = 45;
    public float down_angle = -45;

    private int angle = 0;
    private int face_angle = 0;
    private float speed_rotation = 30;
    private float face_speed_rotation = 20;
    private float actual_angle = 0;
    private float actual_facing_angle = 0;

    private bool rotating = false;
    private bool moving = false;
    private bool face_rotating = false;

    private CompAudio audio;

    private CharactersManager char_manager;

    //2D coordinates, y=z in 3D coordinates

    void Start()
    {
        enemy_obj = GetLinkedObject("enemy_obj"); //Change this to accept more than one enemy

        char_manager = GetComponent<CharactersManager>();

        audio = GetComponent<CompAudio>();
        audio.PlayEvent("PlayMusic");

        curr_dir = (Direction)start_direction;
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();

        array2Da = new int[map_width, map_height];
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                array2Da[x, y] = 0;
            }
        }

        endPosition = GetComponent<Transform>().local_position;
        endRotation = GetComponent<Transform>().local_rotation;

        string map = Map.GetMapString();

        int t = 0;
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                array2Da[x, y] = int.Parse(map[t].ToString());
                t += 1;
            }
        }

        //Search player position
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                if (array2Da[x, y] == 2)
                {
                    curr_x = x;
                    curr_y = y;
                    Debug.Log(curr_x + " " + curr_y);
                    array2Da[x, y] = 0;
                    MovePositionInitial(new Vector3((float)curr_x * distanceToMove, GetComponent<Transform>().local_position.y, (float)curr_y * distanceToMove));
                }
            }
        }
    }

    void Update()
    {
        start_direction = (int)curr_dir;

        CheckIsWalkable();

        // CHARACTER NOT MOVING
        if (GetComponent<Transform>().local_position == endPosition && rotating == false && face_rotating == false)
        {
            tile_mov_x = 0;
            tile_mov_y = 0;
            moving = false;

            // CHECK ROTATION --------------------------
            CheckRotation();

            // CHECK MOVEMENT --------------------------
            CheckMovement();

            // CHECK FACING --------------------------
            CheckFacingRotation();

            //Calculate endPosition
            if ((tile_mov_x != 0 || tile_mov_y != 0) && array2Da[curr_x + tile_mov_x, curr_y + tile_mov_y] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)tile_mov_x, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)tile_mov_y);
                curr_x += tile_mov_x;
                curr_y += tile_mov_y;
                char_manager.SetCurrentPosition();
                moving = true;
            }
        }
        else if (rotating)
        {
            moving = false;
            GetComponent<Transform>().RotateAroundAxis(Vector3.Up, angle * speed_rotation * Time.deltaTime);
            float moved_angle = (float)angle * speed_rotation * Time.deltaTime;
            if (angle < 0)
            {
                actual_angle += (moved_angle * -1);
            }
            else
            {
                actual_angle += moved_angle;
            }

            if (actual_angle >= 90)
            {
                rotating = false;
                if (actual_angle > 90)
                {
                    float marge = actual_angle - 90;
                    if (angle < 0)
                    {
                        GetComponent<Transform>().RotateAroundAxis(Vector3.Up, marge);
                    }
                    else
                    {
                        GetComponent<Transform>().RotateAroundAxis(Vector3.Up, -marge);
                    }
                    char_manager.SetCurrentPosition();
                }
            }
        }
        else if (face_rotating)
        {
            moving = false;
            GetComponent<Transform>().RotateAroundAxis(Vector3.Left, face_angle * face_speed_rotation * Time.DeltaTime());
            float moved_angle = (float)face_angle * face_speed_rotation * Time.DeltaTime();

            if (angle < 0)
            {
                actual_facing_angle += (moved_angle * -1);
            }
            else
            {
                actual_facing_angle += moved_angle;
            }

            if (curr_fac == Facing.UP)
            {
                if (actual_facing_angle >= up_angle)
                {
                    face_rotating = false;
                    if (actual_facing_angle > up_angle)
                    {
                        float marge = actual_facing_angle - up_angle;
                        GetComponent<Transform>().RotateAroundAxis(Vector3.Left, -marge);
                    }
                }
            }
            else if (curr_fac == Facing.STRAIGHT)
            {
                if (actual_facing_angle >= -1.0f && actual_facing_angle <= 1)
                {
                    face_rotating = false;
                }
            }
            else if (curr_fac == Facing.DOWN)
            {
                if (actual_facing_angle <= down_angle)
                {
                    face_rotating = false;
                    if (actual_facing_angle < down_angle)
                    {
                        float marge = actual_facing_angle - down_angle;
                        GetComponent<Transform>().RotateAroundAxis(Vector3.Left, marge);
                    }
                }
            }
        }
        else if (moving)
        {
            GetComponent<Transform>().local_position = Vector3.MoveTowards(GetComponent<Transform>().local_position, endPosition, movSpeed * Time.DeltaTime());
        }
    }

    private void CheckRotation()
    {
        if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().IsIdle())
        {
            if (Input.GetKeyDown(KeyCode.Q)) //Left
            {
                actual_angle = 0;
                angle = -10;
                rotating = true;
                ModificateCurrentDirection(true);
            }
            if (Input.GetKeyDown(KeyCode.E)) //Right
            {
                actual_angle = 0;
                angle = 10;
                rotating = true;
                ModificateCurrentDirection(false);
            }

            float variation = Input.GetInput_ControllerAxis("RHorizontal", "Player");
            if (variation < -0.8)
            {
                actual_angle = 0;
                angle = -10;
                rotating = true;
                ModificateCurrentDirection(true);
                //Debug.Log("AXIS");
            }
            else if (variation > 0.8)
            {
                actual_angle = 0;
                angle = 10;
                rotating = true;
                ModificateCurrentDirection(false);
                //Debug.Log("AXIS");
            }
        }
    }

    private void CheckMovement()
    {
        if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().IsIdle())
        {
            if (Input.GetKeyDown(KeyCode.A) /*&& !EnemyInLeft()*/) //Left
            {
                audio = GetComponent<CompAudio>();
                audio.PlayEvent("Footsteps");
                MoveLeft(out tile_mov_x, out tile_mov_y);
            }
            else if (Input.GetKeyDown(KeyCode.D) /*&& !EnemyInRight()*/) //Right
            {
                audio = GetComponent<CompAudio>();
                audio.PlayEvent("Footsteps");
                MoveRight(out tile_mov_x, out tile_mov_y);
            }
            else if (Input.GetKeyDown(KeyCode.W) /*&& !EnemyInFront()*/) //Up
            {
                audio = GetComponent<CompAudio>();
                audio.PlayEvent("Footsteps");
                MoveForward(out tile_mov_x, out tile_mov_y);
            }
            else if (Input.GetKeyDown(KeyCode.S) /*&& !EnemyBehind()*/) //Down
            {
                audio = GetComponent<CompAudio>();
                audio.PlayEvent("Footsteps");
                MoveBackward(out tile_mov_x, out tile_mov_y);
            }

            float variation = Input.GetInput_ControllerAxis("LHorizontal", "Player");
            if (variation > 0.8)
            {
                audio = GetComponent<CompAudio>();
                audio.PlayEvent("Footsteps");
                MoveRight(out tile_mov_x, out tile_mov_y);
                Debug.Log("Right");
            }
            else if (variation < -0.8)
            {
                audio = GetComponent<CompAudio>();
                audio.PlayEvent("Footsteps");
                MoveLeft(out tile_mov_x, out tile_mov_y);
                Debug.Log("Left");
            }

            float variation2 = Input.GetInput_ControllerAxis("LVertical", "Player");
            if (variation2 > 0.8)
            {
                audio = GetComponent<CompAudio>();
                audio.PlayEvent("Footsteps");
                MoveBackward(out tile_mov_x, out tile_mov_y);
                Debug.Log("Back");
            }
            else if (variation2 < -0.8)
            {
                audio = GetComponent<CompAudio>();
                audio.PlayEvent("Footsteps");
                MoveForward(out tile_mov_x, out tile_mov_y);
                Debug.Log("Forw");
            }
        }
    }

    private void CheckFacingRotation()
    {
        if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().IsIdle())
        {
            if (Input.GetKeyDown(KeyCode.Z)) //Look Up
            {
                if (curr_fac != Facing.UP)
                {
                    if (curr_fac == Facing.STRAIGHT)
                    {
                        curr_fac = Facing.UP;
                        actual_facing_angle = 0;
                    }
                    else if (curr_fac == Facing.DOWN)
                    {
                        curr_fac = Facing.STRAIGHT;
                        actual_facing_angle = down_angle;
                    }
                    face_angle = 10;
                    face_rotating = true;
                }
            }
            else if (Input.GetKeyDown(KeyCode.X)) //Look Down
            {
                if (curr_fac != Facing.DOWN)
                {
                    if (curr_fac == Facing.STRAIGHT)
                    {
                        curr_fac = Facing.DOWN;
                        actual_facing_angle = 0;
                    }
                    else if (curr_fac == Facing.UP)
                    {
                        curr_fac = Facing.STRAIGHT;
                        actual_facing_angle = up_angle;
                    }
                    face_angle = -10;
                    face_rotating = true;
                }
            }
        }
    }

    public void BlockCamera()
    {
        blocked_camera = true;
    }

    public void UnBlockCamera()
    {
        blocked_camera = false;
    }

    void MovePositionInitial(Vector3 newpos)
    {
        GetComponent<Transform>().SetPosition(newpos);
    }

    void ModificateCurrentDirection(bool left)
    {
        if (left)
        {
            if (curr_dir == Direction.NORTH)
            {
                curr_dir = Direction.WEST;
            }
            else if (curr_dir == Direction.EAST)
            {
                curr_dir = Direction.NORTH;
            }
            else if (curr_dir == Direction.SOUTH)
            {
                curr_dir = Direction.EAST;
            }
            else if (curr_dir == Direction.WEST)
            {
                curr_dir = Direction.SOUTH;
            }
        }
        else
        {
            if (curr_dir == Direction.NORTH)
            {
                curr_dir = Direction.EAST;
            }
            else if (curr_dir == Direction.EAST)
            {
                curr_dir = Direction.SOUTH;
            }
            else if (curr_dir == Direction.SOUTH)
            {
                curr_dir = Direction.WEST;
            }
            else if (curr_dir == Direction.WEST)
            {
                curr_dir = Direction.NORTH;
            }
        }
    }

    public void MoveForward(out int tile_mov_x, out int tile_mov_y)
    {
        tile_mov_x = 0;
        tile_mov_y = 0;
        if (curr_dir == Direction.NORTH)
        {
            tile_mov_y = -1;
        }
        if (curr_dir == Direction.EAST)
        {
            tile_mov_x = 1;
        }
        if (curr_dir == Direction.SOUTH)
        {
            tile_mov_y = 1;
        }
        if (curr_dir == Direction.WEST)
        {
            tile_mov_x = -1;
        }
    }

    public void MoveRight(out int tile_mov_x, out int tile_mov_y)
    {
        tile_mov_x = 0;
        tile_mov_y = 0;
        if (curr_dir == Direction.NORTH)
        {
            tile_mov_x = 1;
        }
        if (curr_dir == Direction.EAST)
        {
            tile_mov_y = 1;
        }
        if (curr_dir == Direction.SOUTH)
        {
            tile_mov_x = -1;
        }
        if (curr_dir == Direction.WEST)
        {
            tile_mov_y = -1;
        }
    }

    public void MoveBackward(out int tile_mov_x, out int tile_mov_y)
    {
        tile_mov_x = 0;
        tile_mov_y = 0;
        if (curr_dir == Direction.NORTH)
        {
            tile_mov_y = 1;
        }
        if (curr_dir == Direction.EAST)
        {
            tile_mov_x = -1;
        }
        if (curr_dir == Direction.SOUTH)
        {
            tile_mov_y = -1;
        }
        if (curr_dir == Direction.WEST)
        {
            tile_mov_x = 1;
        }
    }

    public void MoveLeft(out int tile_mov_x, out int tile_mov_y)
    {
        tile_mov_x = 0;
        tile_mov_y = 0;
        if (curr_dir == Direction.NORTH)
        {
            tile_mov_x = -1;
        }
        if (curr_dir == Direction.EAST)
        {
            tile_mov_y = -1;
        }
        if (curr_dir == Direction.SOUTH)
        {
            tile_mov_x = 1;
        }
        if (curr_dir == Direction.WEST)
        {
            tile_mov_y = 1;
        }
    }

    void CheckIsWalkable()
    {
        if (array2Da[curr_x, curr_y] == 1) // no walkable
        {
            int temp_x = curr_x;
            int temp_y = curr_y;
            if (array2Da[temp_x + 1, temp_y] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)1, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)0);
                curr_x += 1;
                curr_y += 0;
            }
            else if (array2Da[temp_x - 1, temp_y] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)-1, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)0);
                curr_x -= 1;
                curr_y += 0;
            }
            else if (array2Da[temp_x, temp_y + 1] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)0, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)1);
                curr_x += 0;
                curr_y += 1;
            }
            else if (array2Da[temp_x, temp_y - 1] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)0, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)-1);
                curr_x += 0;
                curr_y -= 1;
            }
        }
    }

    public bool CheckIsWalkable(int x, int y)
    {
        if (array2Da[x, y] == 1)
        {
            return false;
        }
        return true;
    }

    public bool IsMoving()
    {
        if(moving || rotating)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    public bool EnemyInFront()
    {
        int direction = (int)curr_dir;
        int position_front_x = curr_x;
        int position_front_y = curr_y;

        switch (direction)
        {
            case (int)MovementController.Direction.NORTH:
                {
                    position_front_y -= 1;
                    break;
                }

            case (int)MovementController.Direction.SOUTH:
                {
                    position_front_y += 1;
                    break;
                }

            case (int)MovementController.Direction.EAST:
                {
                    position_front_x += 1;
                    break;
                }

            case (int)MovementController.Direction.WEST:
                {
                    position_front_x -= 1;
                    break;
                }

            default:
                {
                    break;
                }

        }
        if(CheckIsWalkable(position_front_x, position_front_y))
        {
            return false;
        }
        if (GetLinkedObject("player_obj").GetComponent<EnemiesManager>().FindEnemyByTile(position_front_x,position_front_y) != null) 
        {
            return true;
        }

        return false;
    }

    public bool EnemyBehind()
    {
        int direction = (int)curr_dir;
        int position_front_x = curr_x;
        int position_front_y = curr_y;

        switch (direction)
        {
            case (int)MovementController.Direction.NORTH:
                {
                    position_front_y += 1;
                    break;
                }

            case (int)MovementController.Direction.SOUTH:
                {
                    position_front_y -= 1;
                    break;
                }

            case (int)MovementController.Direction.EAST:
                {
                    position_front_x -= 1;
                    break;
                }

            case (int)MovementController.Direction.WEST:
                {
                    position_front_x += 1;
                    break;
                }

            default:
                {
                    break;
                }

        }

        if (GetLinkedObject("player_obj").GetComponent<EnemiesManager>().FindEnemyByTile(position_front_x, position_front_y) != null)
        {
            return true;
        }

        return false;
    }

    public bool EnemyInLeft()
    {
        int direction = (int)curr_dir;
        int position_front_x = curr_x;
        int position_front_y = curr_y;

        switch (direction)
        {
            case (int)MovementController.Direction.NORTH:
                {
                    position_front_x -= 1;
                    break;
                }

            case (int)MovementController.Direction.SOUTH:
                {
                    position_front_x += 1;
                    break;
                }

            case (int)MovementController.Direction.EAST:
                {
                    position_front_y -= 1;
                    break;
                }

            case (int)MovementController.Direction.WEST:
                {
                    position_front_y += 1;
                    break;
                }

            default:
                {
                    break;
                }

        }

        if (GetLinkedObject("player_obj").GetComponent<EnemiesManager>().FindEnemyByTile(position_front_x, position_front_y) != null) 
        {
            return true;
        }

        return false;
    }

    public bool EnemyInRight()
    {
        int direction = (int)curr_dir;
        int position_front_x = curr_x;
        int position_front_y = curr_y;

        switch (direction)
        {
            case (int)MovementController.Direction.NORTH:
                {
                    position_front_x += 1;
                    break;
                }

            case (int)MovementController.Direction.SOUTH:
                {
                    position_front_x -= 1;
                    break;
                }

            case (int)MovementController.Direction.EAST:
                {
                    position_front_y += 1;
                    break;
                }

            case (int)MovementController.Direction.WEST:
                {
                    position_front_y -= 1;
                    break;
                }

            default:
                {
                    break;
                }

        }

        if (GetLinkedObject("player_obj").GetComponent<EnemiesManager>().FindEnemyByTile(position_front_x, position_front_y) != null) 
        {
            return true;
        }

        return false;
    }

    //Pass the x,y vars as arguments to fill them with player coords
    public void GetPlayerPos(out int x, out int y)
    {
        x = curr_x;
        y = curr_y;
    }

    //To check if the player is in the specified tile
    public bool IsPlayerInTile(int x, int y)
    {
        if (x == curr_x && y == curr_y)
        {
            return true;
        }
        return false;
    }

    public void GetPosFromTile(out Vector3 pos, int tile_x, int tile_y)
    {
        pos = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)tile_x, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)tile_y);
    }

    // Two simple methods to modify the walkability map
    public void SetTileWalkability(int x, int y, int value)
    {
        if (x >= 0 && x < map_width && y >= 0 && y < map_height)
        {
            array2Da[x, y] = value;
        }
    }

    public int GetTileWalkability(int x, int y)
    {
        if (x >= 0 && x < map_width && y >= 0 && y < map_height)
        {
            return array2Da[x, y];
        }

        return -1;
    }
}