using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class SwordParticles : CulverinBehaviour
{
    //public GameObject sword_wall_particle;
    public GameObject sword_enemy_particle;

    //CompParticleSystem wall_particle;
    CompParticleSystem enemy_particle;

    CompCollider col;

    bool wall_collision = false;
    bool enemy_collision = false;

    //BLOOD TEXTURE MANAGEMENT
    Material mat;
    public float blood = 0.0f;

    void Start()
    {
        col = GetComponent<CompCollider>();
        wall_collision = false;
        enemy_collision = false;

        //sword_wall_particle = GetLinkedObject("sword_wall_particle");
        //wall_particle = sword_wall_particle.GetComponent<CompParticleSystem>();

        sword_enemy_particle = GetLinkedObject("sword_enemy_particle");
        enemy_particle = sword_enemy_particle.GetComponent<CompParticleSystem>();

        //TO MANAGE SWORD BLOOD TEXTURE
        mat = GetMaterialByName("Jaime Sword");
        mat.SetFloat("blood", blood);
    }

    void Update()
    {
        mat.SetFloat("blood", blood);
    }

    void OnContact()
    {
        col = GetComponent<CompCollider>();
        GameObject obj = col.GetCollidedObject();

        if (obj != null)
        {      
            if (enemy_collision && obj.CompareTag("Enemy"))
            {
                col = GetComponent<CompCollider>();
                Vector3 point = col.GetContactPoint();

                col = GetComponent<CompCollider>();
                Vector3 normal = col.GetContactNormal();

                sword_enemy_particle.GetComponent<Transform>().SetUpVector(normal);
                point = point + normal * 2;
                sword_enemy_particle.GetComponent<Transform>().SetPosition(point);

                enemy_particle = sword_enemy_particle.GetComponent<CompParticleSystem>();
                enemy_particle.ActivateEmission(true);

                //Iterate all childs, they have a ParticleSystem too
                int childs = sword_enemy_particle.ChildCount();
                for (int i = 0; i < childs; i++)
                {
                    sword_enemy_particle.GetChildByIndex(i).GetComponent<CompParticleSystem>().ActivateEmission(true);
                }

                //Disable Enemy Collisions
                EnableEnemyCollision(false);
            }
        }
    }

    public void EnableWallCollision(bool enable)
    {
        wall_collision = enable;
    }

    public void EnableEnemyCollision(bool enable)
    {
        enemy_collision = enable;
    }

    /* MANAGE BLOOD TEXTURE:
        - Damage Enemy -> blood++
        - Attack Miss  -> blood--  
     */
    public void SetBlood(float blood_increase)
    {
        blood += blood_increase;

        if (blood < 0.0f) 
        {
            blood = 0.0f;
        }

        if (blood > 1.0f)
        {
            blood = 1.0f;
        }
    }

}

