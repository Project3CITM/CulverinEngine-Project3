using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;
using CulverinEditor.Pathfinding;

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
    public bool update_rotation = false;
    public float distanceToMove = 10.0f;
    LevelMap level_map;
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
    public GameObject player_enemies_manager;

    public float time_in_memory = 3.0f;
    public int footstep_radius = 1;

    public bool drowning = false;

    public bool push = false;

    void Start()
    {
        level_map = GetLinkedObject("map_obj").GetComponent<LevelMap>();

        char_manager = GetComponent<CharactersManager>();
        characters_camera = GetLinkedObject("characters_camera");
        player_enemies_manager = GetLinkedObject("player_enemies_manager");

        Audio.StopAllSounds();
        audio = GetComponent<CompAudio>();
        audio.PlayEvent("PlayAmbient");
        audio.PlayEvent("PlayMusic");
        Audio.ChangeState("MusicState", "None");

        curr_dir = (Direction)start_direction;
        update_rotation = false;

        map_width = Map.GetWidthMap();
        map_height = Map.GetHeightMap();

        endPosition = GetComponent<Transform>().local_position;
        endRotation = GetComponent<Transform>().local_rotation;

        //SET PLAYER INTO THE CORRECT MAP TILE
        level_map.GetPositionByeValue(out curr_x, out curr_y, 2); //2 = player start position
        level_map.UpdateMap(curr_x, curr_y, 0);
        MovePositionInitial(new Vector3((float)curr_x * distanceToMove, GetComponent<Transform>().local_position.y, (float)curr_y * distanceToMove));

        drowning = false;
    }

    void Update()
    {
        start_direction = (int)curr_dir;

        //Update Forward Vector for rotations
        if (update_rotation)
        {
            update_rotation = false;
            char_manager.SetCurrentPosition();
        }

        CheckIsWalkable();

        if (GetComponent<Transform>().local_position == endPosition && rotating == false && face_rotating == false && char_manager.GetManagerState() != CharactersManager.State.DROWNING && push == false)
        {
            Debug.Log("ENTEEEEER!");

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
                if (level_map.map[curr_x + tile_mov_x, curr_y + tile_mov_y] == 0)
                {
                    audio = GetComponent<CompAudio>();
                    audio.PlayEvent("Footsteps");
                    endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)tile_mov_x, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)tile_mov_y);
                    curr_x += tile_mov_x;
                    curr_y += tile_mov_y;
                    char_manager.SetCurrentPosition();
                    moving = true;
                }
                else if (level_map.map[curr_x + tile_mov_x, curr_y + tile_mov_y] == 3) //Valryian Fire!
                {
                    StatsScore.PuzzleTry();
                    audio = GetComponent<CompAudio>();
                    audio.PlayEvent("Footsteps");
                    endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)tile_mov_x, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)tile_mov_y);
                    curr_x += tile_mov_x;
                    curr_y += tile_mov_y;
                    char_manager.SetCurrentPosition();
                    moving = true;

                    if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().god_mode == false)
                    {
                        GetComponent<CompRigidBody>().UnLockMotion();
                        GetComponent<CompRigidBody>().ApplyImpulse(new Vector3(0.0f, -50.0f, 0.0f));
                        //char_manager.Drown();
                        drowning = true;
                    }
                }
            }
        }
        else if (push == true)
        {
            GetComponent<Transform>().local_position = Vector3.MoveTowards(GetComponent<Transform>().local_position, endPosition, movSpeed * Time.deltaTime);
            if (GetComponent<Transform>().local_position == endPosition)
                push = false;
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
                update_rotation = true;
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
            GetComponent<Transform>().local_position = Vector3.MoveTowards(GetComponent<Transform>().local_position, endPosition, movSpeed * Time.deltaTime);
            GetComponent<Transform>().local_rotation = Vector3.Lerp(new Vector3(GetComponent<Transform>().local_rotation.x, GetComponent<Transform>().local_rotation.y, GetComponent<Transform>().local_rotation.z), new Vector3(GetComponent<Transform>().local_rotation.x, GetComponent<Transform>().local_rotation.y, GetComponent<Transform>().local_rotation.z), (endPosition.Length - GetComponent<Transform>().local_position.Length));
        }

        if (!moving && characters_camera.GetComponent<CompAnimation>().IsAnimationStopped("Idle") && GetLinkedObject("player_obj").GetComponent<CharactersManager>().GetCurrCharacterState() == 0)
        {
            characters_camera.GetComponent<CompAnimation>().PlayAnimationNode("Idle");
        }

    }

    private bool CheckRotation()
    {
        if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().IsIdle() && drowning == false)
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
        if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().IsIdle() && drowning == false)
        {
            /*if(push == true)
            {
                push = false;
                MovePush(out tile_mov_x, out tile_mov_y);
                GetComponent<PerceptionEmitter>().TriggerHearEvent(PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER, time_in_memory, footstep_radius, curr_x, curr_y);
                return true;
            }*/

            float variation = Input.GetInput_ControllerAxis("Horizontal", "Player");
            if (variation > 0.8)
            {
                if (!EnemyInRight())
                {
                    MoveRight(out tile_mov_x, out tile_mov_y);
                    GetComponent<PerceptionEmitter>().TriggerHearEvent(PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER, time_in_memory, footstep_radius, curr_x, curr_y);
                    return true;
                }
            }
            else if (variation < -0.8)
            {
                if (!EnemyInLeft())
                {
                    MoveLeft(out tile_mov_x, out tile_mov_y);
                    GetComponent<PerceptionEmitter>().TriggerHearEvent(PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER, time_in_memory, footstep_radius, curr_x, curr_y);
                    return true;
                }
            }

            float variation2 = Input.GetInput_ControllerAxis("Vertical", "Player");
            if (variation2 > 0.8)
            {
                if (!EnemyBehind())
                {
                    MoveBackward(out tile_mov_x, out tile_mov_y);
                    GetComponent<PerceptionEmitter>().TriggerHearEvent(PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER, time_in_memory, footstep_radius, curr_x, curr_y);
                    return true;
                }

            }
            else if (variation2 < -0.8)
            {
                if (!EnemyInFront())
                {
                    MoveForward(out tile_mov_x, out tile_mov_y);
                    GetComponent<PerceptionEmitter>().TriggerHearEvent(PERCEPTION_EVENT_TYPE.HEAR_WALKING_PLAYER, time_in_memory, footstep_radius, curr_x, curr_y);
                    return true;
                }
            }
        }
        return false;

    }

    private void CheckFacingRotation()
    {
        if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().IsIdle() && drowning == false)
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

    public void MovePositionInitial(Vector3 newpos)
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

    public void MovePush(PathNode obj)
    {
        characters_camera.GetComponent<CompAnimation>().PlayAnimation("Walk");
        endPosition = new Vector3(obj.GetTileX() * distanceToMove, GetComponent<Transform>().local_position.y, obj.GetTileY() * distanceToMove);
        tile_mov_x = obj.GetTileX() - curr_x;
        tile_mov_y = obj.GetTileY() - curr_y;
        curr_x += tile_mov_x;
        curr_y += tile_mov_y;
        char_manager.SetCurrentPosition();
        push = true;
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
        if (level_map.map[curr_x, curr_y] == 1) // no walkable
        {
            int temp_x = curr_x;
            int temp_y = curr_y;
            if (level_map.map[temp_x + 1, temp_y] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)1, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)0);
                curr_x += 1;
                curr_y += 0;
            }
            else if (level_map.map[temp_x - 1, temp_y] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)-1, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)0);
                curr_x -= 1;
                curr_y += 0;
            }
            else if (level_map.map[temp_x, temp_y + 1] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)0, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)1);
                curr_x += 0;
                curr_y += 1;
            }
            else if (level_map.map[temp_x, temp_y - 1] == 0)
            {
                endPosition = new Vector3(GetComponent<Transform>().local_position.x + distanceToMove * (float)0, GetComponent<Transform>().local_position.y, GetComponent<Transform>().local_position.z + distanceToMove * (float)-1);
                curr_x += 0;
                curr_y -= 1;
            }
        }
    }

    public bool CheckIsWalkable(int x, int y)
    {
        if (level_map.map[x, y] == 1)
        {
            return false;
        }
        return true;
    }

    public bool IsMoving()
    {
        if (moving || rotating)
        {
            if (push == true)
                return false;
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

        if (player_enemies_manager.GetComponent<EnemiesManager>().FindEnemyByTile(position_front_x, position_front_y) != null)
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

        if (player_enemies_manager.GetComponent<EnemiesManager>().FindEnemyByTile(position_front_x, position_front_y) != null)
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

        if (player_enemies_manager.GetComponent<EnemiesManager>().FindEnemyByTile(position_front_x, position_front_y) != null)
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

        if (player_enemies_manager.GetComponent<EnemiesManager>().FindEnemyByTile(position_front_x, position_front_y) != null)
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
            level_map.map[x, y] = value;
        }
    }

    public int GetTileWalkability(int x, int y)
    {
        if (x >= 0 && x < map_width && y >= 0 && y < map_height)
        {
            return level_map.map[x, y];
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

    public Vector3 GetForwardDir()
    {
        int direction = (int)curr_dir;
        int position_front_x = 0;
        int position_front_y = 0;

        switch (direction)
        {
            case (int)MovementController.Direction.NORTH:
                {
                    position_front_y = -1;
                    break;
                }

            case (int)MovementController.Direction.SOUTH:
                {
                    position_front_y = 1;
                    break;
                }

            case (int)MovementController.Direction.EAST:
                {
                    position_front_x = 1;
                    break;
                }

            case (int)MovementController.Direction.WEST:
                {
                    position_front_x = -1;
                    break;
                }

            default:
                {
                    break;
                }

        }
        Vector3 forward_dir = new Vector3(position_front_x, 0, position_front_y);
        return forward_dir;
    }

    public Vector3 SetForwardVector()
    {
        int direction = (int)curr_dir;
        int position_front_x = 0;
        int position_front_y = 0;

        switch (direction)
        {
            case (int)MovementController.Direction.NORTH:
                {
                    position_front_y = -1;
                    break;
                }

            case (int)MovementController.Direction.SOUTH:
                {
                    position_front_y = 1;
                    break;
                }

            case (int)MovementController.Direction.EAST:
                {
                    position_front_x = 1;
                    break;
                }

            case (int)MovementController.Direction.WEST:
                {
                    position_front_x = -1;
                    break;
                }

            default:
                {
                    break;
                }

        }
        Vector3 forward_dir = new Vector3(position_front_x, 0, position_front_y);
        return forward_dir;
    }

    public Direction GetPlayerDirection()
    {
        return curr_dir;
    }

    public void CheckDrawning()
    {
        if (level_map.map[curr_x, curr_y] == 3) //Valryian Fire!
        {
            StatsScore.PuzzleTry();

            if (GetLinkedObject("player_obj").GetComponent<CharactersManager>().god_mode == false)
            {
                CompRigidBody rb = GetComponent<CompRigidBody>();
                rb.UnLockMotion();
                rb.ApplyImpulse(new Vector3(0.0f, -50.0f, 0.0f));
                //char_manager.Drown();
                drowning = true;
            }
        }
    }
}