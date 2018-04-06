using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class SwordParticles : CulverinBehaviour
{
    public GameObject sword_wall_particle;
    public GameObject sword_enemy_particle;

    CompParticleSystem wall_particle;
    CompParticleSystem enemy_particle;

    CompCollider col;

    bool wall_collision = false;
    bool enemy_collision = false;

    void Start()
    {
        col = GetComponent<CompCollider>();
        wall_collision = false;
        enemy_collision = false;

        sword_wall_particle = GetLinkedObject("sword_wall_particle");
        sword_enemy_particle = GetLinkedObject("sword_enemy_particle");

        wall_particle = sword_wall_particle.GetComponent<CompParticleSystem>();
        enemy_particle = sword_enemy_particle.GetComponent<CompParticleSystem>();
    }

    void OnContact()
    {
        Debug.Log("--- OnContact ---");
        col = GetComponent<CompCollider>();
        GameObject obj = col.GetCollidedObject();
        Debug.Log("[pink]" + obj.GetTag());

        if (obj != null)
        {
            Debug.Log(obj.GetName());      
            if (enemy_collision && obj.CompareTag("Enemy"))
            {
                Debug.Log("[green] PAM");
                col = GetComponent<CompCollider>();
                Vector3 point = col.GetContactPoint();

                col = GetComponent<CompCollider>();
                Vector3 normal = col.GetContactNormal();

                sword_enemy_particle.GetComponent<Transform>().SetUpVector(normal);
                point = point + normal * 2;
                sword_enemy_particle.GetComponent<Transform>().SetPosition(point);

                enemy_particle = sword_enemy_particle.GetComponent<CompParticleSystem>();
                enemy_particle.ActivateEmission(true);

                //Disable Enemy Collisions
                EnableEnemyCollision(false);

                Debug.Log("Enemy particle ----------------------------");
            }
            //else if (wall_collision && obj.CompareTag("obstacle"))
            //{
            //    Debug.Log("[yellow] PUM");
            //    col = GetComponent<CompCollider>();
            //    Vector3 point = col.GetContactPoint();

            //    col = GetComponent<CompCollider>();
            //    Vector3 normal = col.GetContactNormal();

            //    sword_wall_particle.GetComponent<Transform>().SetUpVector(normal);
            //    sword_wall_particle.GetComponent<Transform>().SetPosition(point);

            //    wall_particle = sword_wall_particle.GetComponent<CompParticleSystem>();
            //    wall_particle.ActivateEmission(true);

            //    //Disable obstacle collisions
            //    EnableWallCollision(false);

            //    Debug.Log("Wall particle ----------------------------");
            //}
        }
    }

    public void EnableWallCollision(bool enable)
    {
        wall_collision = enable;
        if (wall_collision)
        {
            Debug.Log("[orange] Wall collision true");
        }
        else
        {
            Debug.Log("[orange] Wall collision false");
        }
    }

    public void EnableEnemyCollision(bool enable)
    {
        enemy_collision = enable;

        if (enemy_collision)
        {
            Debug.Log("[orange] Enemy collision true");
        }
        else
        {
            Debug.Log("[orange] Enemy collision false");
        }
    }

}

