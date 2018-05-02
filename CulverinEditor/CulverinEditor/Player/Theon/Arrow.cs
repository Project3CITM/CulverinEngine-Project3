using CulverinEditor;
using CulverinEditor.Debug;


public class Arrow : CulverinBehaviour
{
    public float damage = 20;
    public Vector3 speed = Vector3.Zero;
    public bool collision;
    CompRigidBody rb;
    private bool destroyed = false;

    public GameObject arrow_blood_particles;
    public GameObject arrow_sparks_particles;

    public void SetDamage(float set_damage)
    {
        damage = set_damage;
    }

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        destroyed = false;
        Shoot();
        collision = true;
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
        CompCollider col = GetComponent<CompCollider>();
        GameObject collided_obj = col.GetCollidedObject();
       
        Vector3 point = col.GetContactPoint();
        Vector3 normal = col.GetContactNormal();

        // DAMAGE ---
        if (collided_obj != null && destroyed == false)
        {
            /* PLAY AUDIO */
            GetComponent<CompAudio>().PlayEvent("TheonImpact");
            //Lock transform to avoid trespassing more than one collider
            rb.LockTransform();

            // Check the specific enemy in front of you and apply dmg or call object OnContact
            EnemiesManager enemy_manager = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>();
            if (enemy_manager.IsEnemy(collided_obj))
            {
                enemy_manager.ApplyDamage(collided_obj, damage, Enemy_BT.ENEMY_GET_DAMAGE_TYPE.ARROW);
                if (arrow_blood_particles != null)
                {

                    arrow_blood_particles.GetComponent<Transform>().SetUpVector(normal);
                    arrow_blood_particles.GetComponent<Transform>().SetPosition(point);

                    CompParticleSystem arrow_particles_script = arrow_blood_particles.GetComponent<CompParticleSystem>();
                    arrow_particles_script.ActivateEmission(true);
                }
            }
            else
            {
                CompCollider obj_col = collided_obj.GetComponent<CompCollider>();
                if (obj_col != null)
                {
                    obj_col.CallOnContact();
                }
                if (arrow_sparks_particles != null)
                {

                    arrow_sparks_particles.GetComponent<Transform>().SetUpVector(normal);
                    arrow_sparks_particles.GetComponent<Transform>().SetPosition(point);

                    CompParticleSystem wall_particles_script = arrow_sparks_particles.GetComponent<CompParticleSystem>();
                    wall_particles_script.ActivateEmission(true);
                }
            }
        }
        if (destroyed == false)
        {
            Destroy(gameObject);
            destroyed = true;
        }
    }
}

