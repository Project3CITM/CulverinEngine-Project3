using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelFall : CulverinBehaviour
{
    BarrelMovement barrel_mov;

    public GameObject barrel_mov_go;
    CompRigidBody rigid_body;

    void Start()
    {
        barrel_mov_go = GetLinkedObject("barrel_mov_go");
        barrel_mov = barrel_mov_go.GetComponent<BarrelMovement>();
        rigid_body = gameObject.GetComponent<CompRigidBody>();
    }

    void Update()
    {
        barrel_mov = barrel_mov_go.GetComponent<BarrelMovement>();
        if (barrel_mov.restart)
        {
            //CAL FUNCTION
            gameObject.GetComponent<CompRigidBody>().SetAtMaxJointPose();
            gameObject.GetComponent<CompRigidBody>().ResetForce();
            Debug.Log("Test");
        }

    }

    void OnContact()
    {

        CompRigidBody rbody = GetComponent<CompRigidBody>();

        if (rbody != null)
        {
            rbody.RemoveJoint();

        }
    }
}
