using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelFall : CulverinBehaviour
{
    BarrelMovement barrel_mov;

    public GameObject barrel_mov_go;
    CompRigidBody rigid_body;

    void Start()
    {
        barrel_mov = barrel_mov_go.GetComponent<BarrelMovement>();
        rigid_body = GetComponent<CompRigidBody>();
    }

    void Update()
    {
        if (barrel_mov.restart)
        {
            //CAL FUNCTION


            rigid_body.ResetForces();

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
