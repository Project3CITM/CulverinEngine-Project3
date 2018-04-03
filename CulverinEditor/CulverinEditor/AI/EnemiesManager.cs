using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections.Generic;

public class EnemiesManager : CulverinBehaviour
{
    public List<GameObject> sword_enemies = null;
    public List<GameObject> lance_enemies = null;
    public List<GameObject> shield_enemies = null;

    public void Start()
    {
        Debug.Log("[error]Start EnemiesManager");
        sword_enemies = new List<GameObject>();
        lance_enemies = new List<GameObject>();
        shield_enemies = new List<GameObject>();
    }

    public void AddSwordEnemy(GameObject enemy)
    {
        if (sword_enemies == null)
        {
            Debug.Log("[error]EnemiesManager: Sword_enemies is null");
        }
        else
        {
            sword_enemies.Add(enemy);
        }
    }

    public void DeleteSwordEnemy(GameObject enemy)
    {
        if (sword_enemies == null)
        {
            Debug.Log("[error]EnemiesManager: Sword_enemies is null");
        }
        else
        {
            sword_enemies.Remove(enemy);
            Debug.Log("Sword enemy deleted!");
        }
    }

    public void AddLanceEnemy(GameObject enemy)
    {
        if (lance_enemies == null)
        {
            Debug.Log("[error]EnemiesManager: Lance_enemies is null");
        }
        else
        {
            lance_enemies.Add(enemy);
        }
    }

    public void DeleteLanceEnemy(GameObject enemy)
    {
        if (lance_enemies == null)
        {
            Debug.Log("[error]EnemiesManager: Lance_enemies is null");
        }
        else
        {
            lance_enemies.Remove(enemy);
            Debug.Log("Lance enemy deleted!");
        }
    }

    public void AddShieldEnemy(GameObject enemy)
    {
        if (shield_enemies == null)
        {
            Debug.Log("[error]EnemiesManager: Shield_enemies is null");
        }
        else
        {
            shield_enemies.Add(enemy);
            Debug.Log("[error]ShieldEnemy is in the house");
        }
    }

    public void DeleteShieldEnemy(GameObject enemy)
    {
        if (shield_enemies == null)
        {
            Debug.Log("[error]EnemiesManager: Shield_enemies is null");
        }
        else
        {
            shield_enemies.Remove(enemy);
            Debug.Log("Shield enemy deleted!");
        }
    }

    public Enemy_BT FindEnemyByTile(int x, int y)
    {
        int size = sword_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            GameObject enemy_t = sword_enemies[k];
            if (enemy_t != null)
            {
                Movement_Action temp_move = enemy_t.GetComponent<Movement_Action>();
                if (temp_move != null) {
                    int tile_x = temp_move.GetCurrentTileX();
                    int tile_y = temp_move.GetCurrentTileY();
                    if (x == tile_x && y == tile_y)
                    {
                        return enemy_t.GetComponent<EnemySword_BT>();
                    }
                }
                else
                {
                    Debug.Log("[error]No movement action!!!!!");
                }
            }
            else
            {
                Debug.Log("[error]Sowrd enemy null!!!!!");
            }
        }

        size = lance_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            GameObject enemy_t = lance_enemies[k];
            if (enemy_t != null)
            {
                Movement_Action temp_move = enemy_t.GetComponent<Movement_Action>();
                if (temp_move != null)
                {
                    int tile_x = temp_move.GetCurrentTileX();
                    int tile_y = temp_move.GetCurrentTileY();
                    if (x == tile_x && y == tile_y)
                    {
                        return enemy_t.GetComponent<EnemySpear_BT>();
                    }
                }
                else
                {
                    Debug.Log("[error]No movement action!!!!!");
                }
            }
            else
            {
                Debug.Log("[error]Sowrd enemy null!!!!!");
            }
        }

        size = shield_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            GameObject enemy_t = shield_enemies[k];
            if (enemy_t != null)
            {
                Movement_Action temp_move = enemy_t.GetComponent<Movement_Action>();
                if (temp_move != null)
                {
                    int tile_x = temp_move.GetCurrentTileX();
                    int tile_y = temp_move.GetCurrentTileY();
                    if (x == tile_x && y == tile_y)
                    {
                        return enemy_t.GetComponent<EnemyShield_BT>();
                    }
                }
                else
                {
                    Debug.Log("[error]No movement action!!!!!");
                }
            }
            else
            {
                Debug.Log("[error]Sowrd enemy null!!!!!");
            }
        }
        return null;
    }

    public void DamageEnemyInTile(int x,int y, float damage)
    {
        int size = sword_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            int tile_x = sword_enemies[k].GetComponent<Movement_Action>().GetCurrentTileX();
            int tile_y = sword_enemies[k].GetComponent<Movement_Action>().GetCurrentTileY();
            if (x == tile_x && y == tile_y)
            {
                sword_enemies[k].GetComponent<EnemySword_BT>().ApplyDamage(damage);
                return;
            }
        }

        size = lance_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            int tile_x = lance_enemies[k].GetComponent<Movement_Action>().GetCurrentTileX();
            int tile_y = lance_enemies[k].GetComponent<Movement_Action>().GetCurrentTileY();
            if (x == tile_x && y == tile_y)
            {
                lance_enemies[k].GetComponent<EnemySpear_BT>().ApplyDamage(damage);
                return;
            }
        }

        size = shield_enemies.Count;

        for (int k = 0; k < size; k++)
        {
            int tile_x = shield_enemies[k].GetComponent<Movement_Action>().GetCurrentTileX();
            int tile_y = shield_enemies[k].GetComponent<Movement_Action>().GetCurrentTileY();
            if (x == tile_x && y == tile_y)
            {
                shield_enemies[k].GetComponent<EnemyShield_BT>().ApplyDamage(damage);
                return;
            }
        }
    }

    public void ApplyDamage(GameObject target, float damage)
    {
        Debug.Log("Apply Damage");
        if (target.GetComponent<EnemySword_BT>() != null)
        {
            Debug.Log("Sword");
            target.GetComponent<EnemySword_BT>().ApplyDamage(damage);
        }
        else if(target.GetComponent<EnemySpear_BT>() != null)
        {
            Debug.Log("Spear");
            target.GetComponent<EnemySpear_BT>().ApplyDamage(damage);
        }
        else if(target.GetComponent<EnemyShield_BT>() != null)
        {
            Debug.Log("Shield");
            target.GetComponent<EnemyShield_BT>().ApplyDamage(damage);
        }           
    }

    public bool IsEnemy(GameObject target)
    {
        if(target.GetComponent<EnemySword_BT> () != null ||
            target.GetComponent<EnemyShield_BT>() != null ||
            target.GetComponent<EnemySpear_BT>() != null)
        {
            Debug.Log("[error] IS ENEMY");
            return true;
        }
        else
        {
            Debug.Log("[error] IS NOOOOOOOT ENEMY");
            return false;
        }
    }

    public void Push(GameObject target, Vector3 dir)
    {
        if (target.GetComponent<EnemySword_BT>() != null)
        {
            target.GetComponent<EnemySword_BT>().PushEnemy(dir);
        }
        else if (target.GetComponent<EnemySpear_BT>() != null)
        {
            target.GetComponent<EnemySpear_BT>().PushEnemy(dir);
        }
        else if (target.GetComponent<EnemyShield_BT>() != null)
        {
            target.GetComponent<EnemyShield_BT>().PushEnemy(dir);
        }
    }

}

