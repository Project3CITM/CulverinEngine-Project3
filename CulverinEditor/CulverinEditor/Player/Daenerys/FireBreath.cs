using CulverinEditor;
using CulverinEditor.Debug;


public class FireBreath : CulverinBehaviour
{
    public float damage = 50;
    private CompRigidBody rb;
    private bool collided = false;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        collided = false;
    }

    void OnContact()
    {
        if (collided == false)
        {
            GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();

            // DAMAGE ---
            if (collided_obj != null)
            {
                rb = GetComponent<CompRigidBody>();
                //Lock transform to avoid trespassing more than one collider
                rb.LockTransform();

                // Check the specific enemy in front of you and apply dmg or call object OnContact
                EnemiesManager enemy_manager = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>();
                if (enemy_manager.IsEnemy(collided_obj))
                {
                    collided = true;
                    enemy_manager.ApplyDamage(collided_obj, damage, Enemy_BT.ENEMY_GET_DAMAGE_TYPE.FIREBALL);
                }
                else
                {
                    CompCollider obj_col = collided_obj.GetComponent<CompCollider>();
                    if (obj_col != null)
                    {
                        obj_col.CallOnContact();
                    }
                }
            }
        }
    }
}
