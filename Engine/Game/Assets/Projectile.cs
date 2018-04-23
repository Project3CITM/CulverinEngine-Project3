using CulverinEditor;
using CulverinEditor.Debug;


public class Projectile : CulverinBehaviour
{
    public float damage = 10;
    public Vector3 speed = Vector3.Zero;
    public bool collision;
    CompRigidBody rb;
    private bool destroyed = false;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        destroyed = false;
        Shoot();
        collision = true;
        damage = 10.0f;
    }

    public void Shoot()
    {
        rb.UnLockTransform();
        Vector3 force = new Vector3(0, 5, 0);
        rb.ApplyImpulse(force + speed*100); // Forward impulse
        rb.ApplyTorqueForce(speed*40); // Fall force
    }

    void Update()
    {
        if (GetComponent<Transform>().local_position.y < -5 && destroyed == false)
        {
            Destroy(gameObject);
            destroyed = true;
        }

        //if(rb.LockedTransform())
        //{
        //    Destroy(float delay)
        //}
    }

    void OnContact()
    {
        Debug.Log("[yellow]Contact Start");
        CompCollider col = GetComponent<CompCollider>();
        GameObject collided_obj = col.GetCollidedObject();
       
        Vector3 point = col.GetContactPoint();
        Vector3 normal = col.GetContactNormal();

        // DAMAGE ---
        Debug.Log("[error] Collided");
        if (collided_obj != null && destroyed == false)
        {
            /* PLAY AUDIO */
            //GetComponent<CompAudio>().PlayEvent("TheonImpact");
            //Lock transform to avoid trespassing more than one collider
            rb.LockTransform();

            Debug.Log("[yellow] OnContact");
            Debug.Log("[yellow]" + collided_obj.GetName());
        }
        if (destroyed == false)
        {
            Destroy(gameObject);
            destroyed = true;
        }
    }
}

