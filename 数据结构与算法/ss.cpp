/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution
{
public:
    ListNode *mergeTwoLists(ListNode *l1, ListNode *l2)
    {
        if (l1.length == 0 && l2.length == 0)
        {
            return []
        }
        else if (l1.length != 0 && l2.length == 0)
        {
            return l1
        }
        else if (l1.length == 0 && l2.length != 0)
        {
            return l2
        }
        else
        {
            int p = 0, q = 0, m = 0;
            ListNode *l3 = new ListNode();
            m = l3;
            while (p->next != null || q->next != null)
            {
                if (p->next->val >= q->next->val)
                {
                    m->next = q;
                    q = q->next;
                }
                else
                {
                    m->next = p;
                    p = p->next;
                }
                m = m->next;
            }
            if (p->next != null)
            {
                m->next = p;
            }
            else
            {
                m->next = q;
            }
            return m;
        }
    }
};