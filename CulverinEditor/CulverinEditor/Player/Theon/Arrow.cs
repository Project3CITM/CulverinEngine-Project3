using CulverinEditor;
using CulverinEditor.Debug;


public class Arrow : CulverinBehaviour
{
    float damage;
    Vector3 speed;
    Vector3 gravity;
    bool collision;
    CompRigidBody rb;
    CompCollider collider;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        Shoot();
    }

    public void Shoot()
    {
        rb.WakeUp();
        rb = GetComponent<CompRigidBody>();
        rb.UnLockTransform();
        rb.ApplyImpulse(new Vector3(0,0,10)); // Forward impulse
        rb.ApplyTorqueForce(new Vector3(0, 0, 10)); // Fall force
    }

    void Update()
    {
        if(GetComponent<Transform>().local_position.y<0)
        {
            //Destroy();
        }

        //if(rb.LockedTransform())
        //{
        //    Destroy(float delay)
        //}
    }

    void OnTriggerEnter()
    {
        collider = GetComponent<CompCollider>();
        GameObject collided;
        collided = collider.GetCollidedObject();
        
        // DAMAGE ---

        // DESTROY ---
        if(collision)
        {
            //Destroy();
        }
    }

    void OnContact()
    {
        rb.LockTransform();
    }
}

