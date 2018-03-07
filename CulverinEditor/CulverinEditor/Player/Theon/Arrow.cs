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

    }

    public void Shoot()
    {
        rb = GetComponent<CompRigidBody>();
        rb.UnLockTransform();
        rb.ApplyImpulse(speed); // Forward impulse
        rb.ApplyTorqueForce(gravity); // Fall force
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

