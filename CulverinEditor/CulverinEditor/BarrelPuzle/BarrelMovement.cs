using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelMovement : CulverinBehaviour
{

    public GameObject instance;
    BarrelManage manage;
    public GameObject barrel = null;

    float length;
    Transform trans;
    bool test = false;


    void Start()
    {
        instance = GetLinkedObject("instance");
        manage = instance.GetComponent<BarrelManage>();
        trans = gameObject.GetComponent<Transform>();
        barrel = null;

    }
    void Update()
    {
        instance = GetLinkedObject("instance");
        
        manage = instance.GetComponent<BarrelManage>();
       // trans = gameObject.GetComponent<Transform>();

       
        Vector3 diff = gameObject.GetComponent<Transform>().GetPosition() - new Vector3(0,-4,0);
        if (diff.Length >= manage.length)
        {
            Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();
            gameObject.GetComponent<CompRigidBody>().MoveKinematic(new Vector3(0, -4, 0), quat);
          //  gameObject.GetComponent<Transform>().SetPosition( new Vector3(0, -4, 0));
         

        }
        if (!manage.stop)
        {
            Vector3 position = new Vector3(gameObject.GetComponent<Transform>().local_position.x + manage.movSpeed * manage.p_dt, gameObject.GetComponent<Transform>().local_position.y, gameObject.GetComponent<Transform>().local_position.z);
            Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();
            gameObject.GetComponent<CompRigidBody>().MoveKinematic(position,quat);
           // gameObject.GetComponent<Transform>().SetPosition(new Vector3(gameObject.GetComponent<Transform>().local_position.x + manage.movSpeed * manage.p_dt, gameObject.GetComponent<Transform>().local_position.y, gameObject.GetComponent<Transform>().local_position.z));

        }

        else
        {
            gameObject.GetComponent<Transform>().SetPosition(new Vector3(Mathf.Round(gameObject.GetComponent<Transform>().local_position.x), gameObject.GetComponent<Transform>().local_position.y, gameObject.GetComponent<Transform>().local_position.z));

        }

    
    }

    uint lfsr = 0xACE1u;
    uint bit;

    uint rand()
    {
        bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
        return lfsr = (lfsr >> 1) | (bit << 15);
    }
}