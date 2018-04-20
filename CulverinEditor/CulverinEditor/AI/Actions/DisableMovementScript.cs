using CulverinEditor.Debug;
using CulverinEditor;

public class DisableMovementScript : CulverinBehaviour
{
    public GameObject sword_enemy;
    public GameObject spear_enemy;
    public GameObject shield_enemy;


    public void Start()
    {
        sword_enemy = GetLinkedObject("sword_enemy");
        spear_enemy = GetLinkedObject("spear_enemy");
        shield_enemy = GetLinkedObject("shield_enemy");
    }

    public void Update()
    {
        
        
        if (Input.GetKeyDown(KeyCode.J))
        {
            Debug.Log("Disable Sword Enemy", Department.IA, Color.PINK);
            if (sword_enemy.GetComponent<EnemySword_BT>() != null && sword_enemy.GetComponent<Seek_Steering>() != null)
            {
                Debug.Log("Has scripts", Department.IA, Color.PINK);
                if (sword_enemy.GetComponent<EnemySword_BT>().Disable_Movement_Gameplay_Debbuger==false)
                {
                    Debug.Log("Is enable", Department.IA, Color.PINK);
                    sword_enemy.GetComponent<EnemySword_BT>().Disable_Movement_Gameplay_Debbuger = true;
                    sword_enemy.GetComponent<Seek_Steering>().SetEnabled(false);
                }
                else
                {
                    Debug.Log("Is not enable", Department.IA, Color.PINK);
                    sword_enemy.GetComponent<EnemySword_BT>().Disable_Movement_Gameplay_Debbuger = false;
                    sword_enemy.GetComponent<Seek_Steering>().SetEnabled(true);
                }
       
            }
            else
            {
                Debug.Log("No scripts", Department.IA, Color.PINK);
            }
        }

        if (Input.GetKeyDown(KeyCode.K))
        {
            Debug.Log("Disable Shield Enemy", Department.IA, Color.PINK);
            if (shield_enemy.GetComponent<EnemyShield_BT>() != null && shield_enemy.GetComponent<Seek_Steering>() != null)
            {
                Debug.Log("Has scripts", Department.IA, Color.PINK);
                if (shield_enemy.GetComponent<EnemyShield_BT>().Disable_Movement_Gameplay_Debbuger == false)
                {
                    Debug.Log("Is enable", Department.IA, Color.PINK);
                    shield_enemy.GetComponent<EnemyShield_BT>().Disable_Movement_Gameplay_Debbuger = true;
                    shield_enemy.GetComponent<Seek_Steering>().SetEnabled(false);
                }
                else
                {
                    Debug.Log("Is not enable", Department.IA, Color.PINK);
                    shield_enemy.GetComponent<EnemyShield_BT>().Disable_Movement_Gameplay_Debbuger = false;
                    shield_enemy.GetComponent<Seek_Steering>().SetEnabled(true);
                }

            }
            else
            {
                Debug.Log("No scripts", Department.IA, Color.PINK);
            }
        }

        if (Input.GetKeyDown(KeyCode.L))
        {
            Debug.Log("Disable Spear Enemy", Department.IA, Color.PINK);
            if (spear_enemy.GetComponent<EnemySpear_BT>() != null && spear_enemy.GetComponent<Seek_Steering>() != null)
            {
                Debug.Log("Has scripts", Department.IA, Color.PINK);
                if (spear_enemy.GetComponent<EnemySpear_BT>().Disable_Movement_Gameplay_Debbuger == false)
                {
                    Debug.Log("Is enable", Department.IA, Color.PINK);
                    spear_enemy.GetComponent<EnemySpear_BT>().Disable_Movement_Gameplay_Debbuger = true;
                    spear_enemy.GetComponent<Seek_Steering>().SetEnabled(false);
                }
                else
                {
                    Debug.Log("Is not enable", Department.IA, Color.PINK);
                    spear_enemy.GetComponent<EnemySpear_BT>().Disable_Movement_Gameplay_Debbuger = false;
                    spear_enemy.GetComponent<Seek_Steering>().SetEnabled(true);
                }

            }
            else
            {
                Debug.Log("No scripts", Department.IA, Color.PINK);
            }
        }

    }



   }