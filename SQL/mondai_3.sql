-- 直近30日間にカート追加を3回以上行い、同期間に注文していない顧客を取得する。
-- Retrieve customers who added items to their cart at least three times but placed no orders in the last 30 days.

select c.cid
from customers as c
join add2cart as a1 on a1.cid = c.cid
join add2cart as a2 on a2.cid = c.cid and a1.ts < a2.ts and a1.ts <> a2.ts
join add2cart as a3 on a3.cid = c.cid and a2.ts < a3.ts and a2.ts <> a3.ts and a1.ts <> a3.ts
left join payment as p on p.cid = c.cid
where (julianday('now') - julianday(a1.ts)) <= 30
except
select distinct os.cid
from orderSessions as os
join orders as o on os.ono = o.ono
where (julianday('now') - julianday(o.odate)) <= 30;