using CulverinEditor;
using CulverinEditor.Debug;

public class FireWall : CulverinBehaviour
{

    public float damage_per_second_percentage = 10.0f;
    public float duration = 5;

    private float timer = 0;
    EnemyController enemy_on_fire;
    // Use this for initialization
    void Start()
    {
        GetComponent<CompParticleSystem>().ActivateEmission(true);
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.DeltaTime();
        if(timer >= duration)
        {
            GetComponent<CompParticleSystem>().ActivateEmission(false);
            Destroy(gameObject);
        }
    }

    void SetFireWall(int x, int y)
    {

    }
}
