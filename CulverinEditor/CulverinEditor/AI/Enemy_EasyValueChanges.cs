using CulverinEditor;
using CulverinEditor.Debug;

public class Enemy_EasyValueChanges : CulverinBehaviour
{
    public float vel_rotation = 0.05f;
    public float vel_movement = 40.0f;
    public float total_hp = 70.0f;
    public float attack_damage = 4.0f;
    public float attack_cooldown = 1.0f;

    Align_Steering align= null;
    Enemy_BT bt = null;
    Movement_Action movement = null;

    public void Start()
    {

        align = gameObject.GetComponent<Align_Steering>();
        movement = gameObject.GetComponent<Movement_Action>();

        if (gameObject.GetComponent<EnemySword_BT>() != null)
        {
            bt = gameObject.GetComponent<EnemySword_BT>();
        }
        else if (gameObject.GetComponent<EnemySpear_BT>() != null)
        {
            Debug.Log("SPEAR ENEMY!", Department.IA, Color.PINK);
            bt = gameObject.GetComponent<EnemySpear_BT>();
        }
        else if (gameObject.GetComponent<EnemyShield_BT>() != null)
        {
            bt = gameObject.GetComponent<EnemyShield_BT>();
        }

        if (bt != null)
        {
            bt.current_hp = total_hp;
            bt.total_hp = total_hp;
            bt.attack_damage = attack_damage;
            bt.attack_cooldown = attack_cooldown;
            Debug.Log("attack cooldown: " + bt.attack_cooldown, Department.IA, Color.PINK);
        }
    }

    public void Update()
    {

        if(align!=null)
        align.SetRotationVel(vel_rotation);

        if (movement != null)
            movement.max_accel = vel_movement;

        if (gameObject.GetComponent<EnemySword_BT>() != null)
        {
            bt = gameObject.GetComponent<EnemySword_BT>();
        }
        else if (gameObject.GetComponent<EnemySpear_BT>() != null)
        {
            Debug.Log("SPEAR ENEMY!", Department.IA, Color.PINK);
            bt = gameObject.GetComponent<EnemySpear_BT>();
        }
        else if (gameObject.GetComponent<EnemyShield_BT>() != null)
        {
            bt = gameObject.GetComponent<EnemyShield_BT>();
        }

        if (bt != null)
        {
            bt.total_hp = total_hp;
            bt.attack_damage = attack_damage;
            bt.attack_cooldown = attack_cooldown;
            Debug.Log("attack cooldown: " + bt.attack_cooldown, Department.IA,Color.PINK);

        }
        else
        {
            Debug.Log("BT NULL", Department.IA, Color.PINK);
        }
    }
}
