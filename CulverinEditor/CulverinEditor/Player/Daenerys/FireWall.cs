using CulverinEditor;
using CulverinEditor.Debug;

public class FireWall : CulverinBehaviour
{
    public float damage_per_second_percentage = 10.0f;
    public float duration = 5f;
    public float damage_tick = 1f;

    private float timer = 0;
    private float damage_timer = 0;
    private int tile_x = 0;
    private int tile_y = 0;
    // Use this for initialization
    void Start()
    {
        GetComponent<CompParticleSystem>().ActivateEmission(true);
    }

    // Update is called once per frame
    void Update()
    {      
        timer += Time.deltaTime;
        damage_timer -= Time.deltaTime;
        if (timer >= duration)
        {
            GetComponent<CompParticleSystem>().ActivateEmission(false);
            Destroy(gameObject);
        }

        Enemy_BT enemy_in_tile = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>().FindEnemyByTile(tile_x, tile_y);
        if (enemy_in_tile != null && damage_timer <= 0f)
        {
            GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>().DamageEnemyInTile(tile_x, tile_y, damage_per_second_percentage);
            damage_timer = damage_tick;
        }
    }

    public void SetTiles (int x, int y)
    {
        tile_x = x;
        tile_y = y;
    }
}
