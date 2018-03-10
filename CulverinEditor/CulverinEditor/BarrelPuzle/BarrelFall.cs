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
                Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();

                BarrelManage manage = barrel_mov.instance.GetComponent<BarrelManage>();
                Vector3 parent_pos = new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z);

                CompRigidBody rigid = gameObject.GetComponent<CompRigidBody>();
                rigid.MoveKinematic(start_pos * 13 + parent_pos, quat);

                rigid.ResetForce();
                rigid.ApplyImpulse(new Vector3(1, 0, 0));

                Debug.Log(start_pos.ToString());
            }
        }

        if (falling)
        {
            //ONE WAY TO CONTROL WHERE THE BARRELS FALL
            BarrelManage manage = barrel_mov.instance.GetComponent<BarrelManage>();
            Vector3 parent_pos = new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z);
            Vector3 actual_pos = gameObject.GetComponent<Transform>().local_position*13 + parent_pos;
            CompRigidBody rigid = gameObject.GetComponent<CompRigidBody>();
            //ADD THIS X POSITION
            Vector3 pos;
            if ((actual_pos.x) - final_x_pos > 2.0f || (actual_pos.x) - final_x_pos < -2.0f)
            {
                pos = new Vector3(actual_pos.x + (final_x_pos - actual_pos.x)/10.0f, actual_pos.y - 2f, actual_pos.z);
            }
            else
            {
                pos = new Vector3(actual_pos.x, actual_pos.y - 0.1f, actual_pos.z);
            }

            Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();
            if (actual_pos.y > -15.0f)
            {
                rigid.MoveKinematic(pos, quat);
            }
            else
            {
                actual_pos.x = final_x_pos;
                rigid.MoveKinematic(actual_pos, quat);
                rigid.LockTransform();
                falling = false;
                move_done = true;
                Debug.Log("Locked");
            }
        }
    }

    void OnContact()
    {
        if (rigid_body != null)
        {
            rigid_body.RemoveJoint();
            falling = true;

            Quaternion quat = rigid_body.GetColliderQuaternion();
            BarrelManage nmana = barrel_mov.instance.GetComponent<BarrelManage>();
            Vector3 parent_pos = new Vector3(nmana.restart_pos_x, nmana.restart_pos_y, nmana.restart_pos_z);
    

            start_x_pos = transform.local_position.x * 13 + parent_pos.x;
            final_x_pos = Mathf.Round((Mathf.Round(transform.local_position.x/2)*2)+1 * 13 + parent_pos.x);
        }
    }
}
