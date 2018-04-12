using CulverinEditor;
using CulverinEditor.Debug;


public class Projectile : CulverinBehaviour
{
    public float damage = 10;
    public Vector3 speed = Vector3.Zero;
    public bool collision;
    CompRigidBody rb;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        Shoot();
        collision = true;
        damage = 10.0f;
    }

    public void Shoot()
    {
        rb.ApplyImpulse(speed * 100);
    }

    void Update()
    {

    }

    void OnContact()
    {
        CompCollider col = GetComponent<CompCollider>();
        GameObject collided_obj = col.GetCollidedObject();
       
        Debug.Log(collided_obj.GetTag().ToString());
        if (collided_obj != null && collided_obj.CompareTag("player"))
        {
            //collided_obj.GetComponent<CharactersManager>().GetDamage(10);
        }
        Destroy(gameObject);
    }
}

