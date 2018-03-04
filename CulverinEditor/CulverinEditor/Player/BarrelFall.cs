using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelFall : CulverinBehaviour
{
    void OnContact()
    {
        Debug.Log("OnContact script");
        CompRigidBody rbody = GetComponent<CompRigidBody>();
        Debug.Log("got joint");
        if (rbody != null)
        {
            Debug.Log("ReadytoDelete");
            rbody.RemoveJoint();
            Debug.Log("JointDeleted");
        }
    }
}

