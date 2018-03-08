using CulverinEditor;
using CulverinEditor.Debug;

public class FireWall : CulverinBehaviour
{

    public float damage_per_second_percentage = 10.0f;
    public float duration = 5;

    private float timer = 0;
    CompParticleSystem particle_fire_wall;
    EnemyController enemy_on_fire;
    // Use this for initialization
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.DeltaTime();
        if(timer >= duration)
        {
            particle_fire_wall.ActivateEmission(false);
            //Destroy();
        }
    }

    void SetFireWall(int x, int y)
    {
        particle_fire_wall.ActivateEmission(true);
        Vector3 pos;
    }
}
