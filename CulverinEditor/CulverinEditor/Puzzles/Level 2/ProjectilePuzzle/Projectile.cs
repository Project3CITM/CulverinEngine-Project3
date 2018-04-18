using CulverinEditor;
using CulverinEditor.Debug;


public class Projectile : CulverinBehaviour
{
    public float damage = 10f;
    public Vector3 speed = Vector3.Zero;
    public bool collision;
    CompRigidBody rb;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        Shoot();
        collision = true;
     
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
        Debug.Log("CONTACT", Department.STAGE, Color.RED);
        CompCollider col = GetComponent<CompCollider>();
        GameObject collided_obj = col.GetCollidedObject();
       
       
        if (collided_obj != null)
        {
            CharactersManager cm = collided_obj.GetComponent<CharactersManager>();

            if (cm != null)
            {
                cm.GetDamage(damage);
            }
         }
        else Debug.Log("Collided obj NULL", Department.STAGE, Color.ORANGE);

        Destroy(gameObject);
    }
}

