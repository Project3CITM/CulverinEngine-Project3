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

    public Direction curr_dir = Direction.NORTH;
    public Facing curr_fac = Facing.STRAIGHT;
    public int start_direction = 1;
    public float movSpeed = 1.0f;
    public Vector3 endPosition;
    public Vector3 endRotation;
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
    public bool moving = false;
    private bool face_rotating = false;

    private CompAudio audio;

    private CharactersManager char_manager;
    public GameObject characters_camera;

    void Start()
    {
        char_manager = GetComponent<CharactersManager>();
        characters_camera = GetLinkedObject("characters_camera");

        audio = GetComponent<CompAudio>();
        audio.PlayEvent("PlayMusic");
        Debug.Log("Play Music");

        curr_dir = (Direction)start_direction;
        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();

        Debug.Log("Get Map");
        Debug.Log(map_width);
        Debug.Log(map_height);

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

        Debug.Log("End Position");

        string map = Map.GetMapString();
        Debug.Log(map);

        int t = 0;
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                array2Da[x, y] = int.Parse(map[t].ToString());
                t += 1;
            }
        }

        Debug.Log("Parse Map");

        //Search player position
        for (int y = 0; y < map_height; y++)
        {
            for (int x = 0; x < map_width; x++)
            {
                if (array2Da[x, y] == 2)
                {
                    curr_x = x;
                    curr_y = y;
                    array2Da[x, y] = 0;
                    MovePositionInitial(new Vector3((float)curr_x * distanceToMove, GetComponent<Transform>().local_position.y, (float)curr_y * distanceToMove));
                }
            }
        }

        Debug.Log("Move initial popsition");

        //intro = GetLinkedObject("intro");
        //intro.SetActive(true);
        //lore_screen = GetLinkedObject("lore_screen");
        //lore_screen.SetActive(false);
        //Time.timeScale = 0;
    }

    void Update()
    {
        //if (intro.IsActive() || lore_screen.IsActive())
        //{
        //    if (Input.GetKeyDown(KeyCode.Space)|| Input.GetInput_KeyDown("Interact", "Player"))
        //    {
        //        if (intro.IsActive())
        //        {
        //            Debug.Log("From intro to lore");
        //            intro.SetActive(false);
        //            lore_screen.SetActive(true);
        //        }
        //        else if (lore_screen.IsActive())
        //        {
        //            Debug.Log("From lore to game");
        //            lore_screen.SetActive(false);
        //            Time.timeScale = 1;
        //        }
        //    }
        //}
        start_direction = (int)curr_dir;

        CheckIsWalkable();

        if (GetComponent<Transform>().local_position == endPosition && rotating == false && face_rotating == false && char_manager.GetManagerState() != CharactersManager.State.DROWNING)
        {
            tile_mov_x = 0;
            tile_mov_y = 0;

            moving = false;

            // CHECK ROTATION --------------------------
            if (!CheckRotation())
            {
                // CHECK MOVEMENT --------------------------
                CheckMovement();
            }
            // CHECK FACING --------------------------
            //CheckFacingRotation();

            //Calculate endPosition
            if ((tile_mov_x != 0 || tile_mov_y != 0))
            {
                if (array2Da[curr_x + tile_mov_x, curr_y + tile_mov_y] == 0)
                {
                    audio = GetComponent<CompAudio>();
                    audio.PlayEvent("Footsteps");
                    endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)tile_mov_x, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)tile_mov_y);
                    curr_x += tile_mov_x;
                    curr_y += tile_mov_y;
                    char_manager.SetCurrentPosition();
                    moving = true;
                }
                else if (array2Da[curr_x + tile_mov_x, curr_y + tile_mov_y] == 3) //Valryian Fire!
                {
                    audio = GetComponent<CompAudio>();
                    audio.PlayEvent("Footsteps");
                    endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)tile_mov_x, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)tile_mov_y);
                    curr_x += tile_mov_x;
                    curr_y += tile_mov_y;
                    char_manager.SetCurrentPosition();
                    moving = true;
                    GetComponent<CompRigidBody>().UnLockTransform();
                    //char_manager.Drown();
                }
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
                }
                char_manager.SetCurrentPosition();
            }
        }
        else if (face_rotating)
        {
            moving = false;
            GetComponent<Transform>().RotateAroundAxis(Vector3.Left, face_angle * face_speed_rotation * Time.deltaTime);
            float moved_angle = (float)face_angle * face_speed_rotation * Time.deltaTime;

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
            GetComponent<Transform>().local_rotation = Vector3.Lerp(new Vector3(GetComponent<Transform>().local_rotation.x, GetComponent<Transform>().local_rotation.y, GetComponent<Transform>().local_rotation.z), new Vector3(GetComponent<Transform>().local_rotation.x, GetComponent<Transform>().local_rotation.y, GetComponent<Transform>().local_rotation.z), (endPosition.Length - GetComponent<Transform>().local_position.Length));
        }
    }

    private bool CheckRotation()
    {
        if (1 == 1/*GetLinkedObject("player_obj").GetComponent<CharactersManager>().IsIdle()*/)
        {
      

            float variation = Input.GetInput_ControllerAxis("Rotate", "Player");
            if (variation < -0.8)
            {
                actual_angle = 0;
                angle = 10;
                rotating = true;
                ModificateCurrentDirection(true);
                return true;

            }
            else if (variation > 0.8)
            {
                actual_angle = 0;
                angle = -10;
                rotating = true;
                ModificateCurrentDirection(false);
                return true;

            }
        }
        return false;

    }

    private bool CheckMovement()
    {
        if (1 == 1/*GetLinkedObject("player_obj").GetComponent<CharactersManager>().IsIdle()*/)
        {
            float variation = Input.GetInput_ControllerAxis("Horizontal", "Player");
            if (variation > 0.8)
            {
                MoveRight(out tile_mov_x, out tile_mov_y);
                return true;
            }
            else if (variation < -0.8)
            {
                MoveLeft(out tile_mov_x, out tile_mov_y);
                return true;
            }

            float variation2 = Input.GetInput_ControllerAxis("Vertical", "Player");
            if (variation2 > 0.8)
            {
                if (/*!EnemyBehind()*/1 == 1)
                {

                    MoveBackward(out tile_mov_x, out tile_mov_y);
                    return true;
                }

            }
            else if (variation2 < -0.8)
            {
                    if (/*!EnemyInFront()*/1 == 1)
                    {

                        MoveForward(out tile_mov_x, out tile_mov_y);
                        return true;
                    }
            }
        }
        return false;

    }

    private void CheckFacingRotation()
    {
        if (1 == 1/*GetLinkedObject("player_obj").GetComponent<CharactersManager>().IsIdle()*/)
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
        characters_camera.GetComponent<CompAnimation>().PlayAnimation("Walk");
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
        characters_camera.GetComponent<CompAnimation>().PlayAnimation("Walk");
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
        characters_camera.GetComponent<CompAnimation>().PlayAnimation("Walk");
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
        characters_camera.GetComponent<CompAnimation>().PlayAnimation("Walk");
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
        if (moving || rotating)
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
        if (CheckIsWalkable(position_front_x, position_front_y))
        {
            return false;
        }
        if (GetLinkedObject("player_obj").GetComponent<EnemiesManager>().FindEnemyByTile(position_front_x, position_front_y) != null)
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

    public void SetMovement(int destination_x, int destination_y)
    {
        int happy_x = destination_x - curr_x;
        int happy_y = destination_y - curr_y;
        curr_x = destination_x;
        curr_y = destination_y;
        endPosition = new Vector3(GetComponent<Transform>().local_position.x + (float)happy_x * distanceToMove, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z - (float)happy_y * distanceToMove);

        GetComponent<Transform>().SetPosition(new Vector3(GetComponent<Transform>().local_position.x + (float)happy_x * distanceToMove, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z - (float)happy_y * distanceToMove));
    }
}