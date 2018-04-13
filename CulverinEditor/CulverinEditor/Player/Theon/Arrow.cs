using CulverinEditor;
using CulverinEditor.Debug;


public class Arrow : CulverinBehaviour
{
    public float damage = 20;
    public Vector3 speed = Vector3.Zero;

    public float to_destroy_time = 0;
    CompRigidBody rb;
    private bool to_destroy = false;

    public GameObject arrow_blood_particles;
    public GameObject arrow_sparks_particles;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        to_destroy = false;
        Shoot();
        damage = 10.0f;
    }

    public void Shoot()
    {
        rb.UnLockTransform();
        speed = speed * 100;
        speed.y += 5;
        rb.ApplyImpulse(speed); // Forward impulse
        rb.ApplyTorqueForce(speed*40); // Fall force
    }

    void Update()
    {
        if(to_destroy)
        {
            to_destroy_time -= Time.deltaTime;
            if (to_destroy_time <= 0)
            {
                Destroy(gameObject);
                to_destroy = false;
            }
        }
        else if (GetComponent<Transform>().local_position.y < -5)
        {
            Destroy(gameObject);
            to_destroy = true;
        }
    }

    void OnContact()
    {
        if(to_destroy)
        {
            return;
        }

        CompCollider col = GetComponent<CompCollider>();
        GameObject collided_obj = col.GetCollidedObject();
       
        Vector3 point = col.GetContactPoint();
        Vector3 normal = col.GetContactNormal();
        col.CollisionActive(false);

        // DAMAGE ---
        if (collided_obj != null)
        {
            to_destroy = true;
            /* PLAY AUDIO */
            GetComponent<CompAudio>().PlayEvent("TheonImpact");
            //Lock transform to avoid trespassing more than one collider

            // Check the specific enemy in front of you and apply dmg or call object OnContact
            EnemiesManager enemy_manager = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>();
            if (enemy_manager.IsEnemy(collided_obj))
            {
                enemy_manager.ApplyDamage(collided_obj, damage);
                if (arrow_blood_particles != null)
                {

                    arrow_blood_particles.GetComponent<Transform>().SetUpVector(normal);
                    arrow_blood_particles.GetComponent<Transform>().SetPosition(point);

                    CompParticleSystem arrow_particles_script = arrow_blood_particles.GetComponent<CompParticleSystem>();
                    arrow_particles_script.ActivateEmission(true);

                    Destroy(gameObject);
                    Debug.Log("[pink] ---------------- BLOOOOOD -------------------");

                }
            }
            else
            {
                if (arrow_sparks_particles != null)
                {
                    arrow_sparks_particles.GetComponent<Transform>().SetUpVector(normal);
                    arrow_sparks_particles.GetComponent<Transform>().SetPosition(point);

                    CompParticleSystem wall_particles_script = arrow_sparks_particles.GetComponent<CompParticleSystem>();
                    wall_particles_script.ActivateEmission(true);

                    normal = normal * 15;
                    normal.y -= 4;
                    GetComponent<CompRigidBody>().ApplyImpulse(normal);

                }
            }
        }
    }
}

