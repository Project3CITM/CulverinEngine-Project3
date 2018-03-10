using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;
public class BarrelFall : CulverinBehaviour
{
    BarrelMovement barrel_mov;

    public GameObject barrel_mov_go;
    public GameObject puzzle_generator_go;
    private BarrelPuzzleGenerator puzzle_generator;
    CompRigidBody rigid_body;
    Vector3 start_pos;
    bool falling = false;
    
    float fall_x_pos = 0;
    float final_x_pos = 0;
    float start_x_pos = 0;
    bool move_done = false;


    void Start()
    {
        barrel_mov_go = GetLinkedObject("barrel_mov_go");
        barrel_mov = barrel_mov_go.GetComponent<BarrelMovement>();
        rigid_body = gameObject.GetComponent<CompRigidBody>();
        start_pos = gameObject.GetComponent<Transform>().local_position;
        Debug.Log(start_pos.ToString());
        falling = false;
        move_done = false;
    }

    void Update()
    {

        if (!move_done && !falling)
        {
            barrel_mov = barrel_mov_go.GetComponent<BarrelMovement>();
            if (barrel_mov.restart)
            {
                //CAL FUNCTION
                // Vector3 pos = gameObject.GetComponent<CompRigidBody>().GetMaxJointPose();
                Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();

                BarrelManage manage = barrel_mov.instance.GetComponent<BarrelManage>();
                Vector3 parent_pos = new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z);

                CompRigidBody rigid = gameObject.GetComponent<CompRigidBody>();
                rigid.MoveKinematic(start_pos * 10 + parent_pos, quat);

                rigid.ResetForce();
                rigid.ApplyImpulse(new Vector3(1, 0, 0));

                Debug.Log(start_pos.ToString());
            }

            //TESTING BARRELS FALL


            if (Input.GetInput_KeyDown("RAttack", "Player"))
            {


                gameObject.GetComponent<CompRigidBody>().RemoveJoint();

                Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();
                BarrelManage manage = barrel_mov.instance.GetComponent<BarrelManage>();
                Vector3 parent_pos = new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z);

                start_x_pos = gameObject.GetComponent<Transform>().local_position.x * 13 + parent_pos.x;
                fall_x_pos = Mathf.Round(start_x_pos) - start_x_pos;
                final_x_pos = Mathf.Round(Mathf.Round(gameObject.GetComponent<Transform>().local_position.x/2.0f) * 26 + parent_pos.x);
                Debug.Log((Mathf.Round(gameObject.GetComponent<Transform>().local_position.x / 2.0f) * 2).ToString());
                falling = true;
            }
        }
        else if (!move_done)
        {
            //ONE WAY TO CONTROL WHERE THE BARRELS FALL
            BarrelManage manage = barrel_mov.instance.GetComponent<BarrelManage>();
            Vector3 parent_pos = new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z);
            Vector3 actual_pos = gameObject.GetComponent<Transform>().local_position*13 + parent_pos;
            CompRigidBody rigid = gameObject.GetComponent<CompRigidBody>();
            //ADD THIS X POSITION
            Vector3 pos;
            if ((actual_pos.x) - final_x_pos > 0.1f || (actual_pos.x) - final_x_pos < -0.1f)
            {
                pos = new Vector3(actual_pos.x + (final_x_pos - actual_pos.x)/20.0f, actual_pos.y - 0.1f, actual_pos.z);
            }
            else
            {
                pos = new Vector3(actual_pos.x, actual_pos.y - 0.1f, actual_pos.z);
            }

            Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();

            if (actual_pos.y > -20.0f)
            {
                Debug.Log("YAYY!!!");
                rigid.MoveKinematic(pos, quat);
            }
            else
            {
                Debug.Log("NOT YAY!!");
                rigid.MoveKinematic(actual_pos, quat);
                rigid.LockTransform();
                move_done = true;
            }
        }



    }
    void OnContact()
    {

        CompRigidBody rbody = GetComponent<CompRigidBody>();
        Debug.Log("Enter Before");
        if (rbody != null)
        {
            Debug.Log("Enter");
            rbody.RemoveJoint();
            puzzle_generator.OnBarrelFall(gameObject);
        }
    }
}
