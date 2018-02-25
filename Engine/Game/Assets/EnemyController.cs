using CulverinEditor;
using CulverinEditor.Debug;

public class EnemyController : CulverinBehaviour
{
    // STATS ---------
    public float hp = 0.0f;
    public float attack_dmg = 0.0f;
    public float mov_speed = 1.0f; // Seconds to pass between movement
    // ---------------

    void Start()
    {

    }

    void Update()
    {
    }

    public void Hit(float damage)
    {
        hp -= damage;
        GetComponent<CompAudio>().PlayEvent("FleshCut");
        Debug.Log("Hit. Curr HP -> " + hp.ToString());
    }
}

