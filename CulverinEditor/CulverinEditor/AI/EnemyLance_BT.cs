using CulverinEditor;
using CulverinEditor.Debug;

class EnemyLance_BT : Enemy_BT
{
    public override void Start()
    {
        GetLinkedObject("enemies_manager").GetComponent<EnemiesManager>().AddLanceEnemy(gameObject);

        base.Start();
    }

    public override void Update()
    {
        base.Update();
    }

    public override void MakeDecision()
    {
        Debug.Log("MAKING LANCE ACTION!");
    }
}

