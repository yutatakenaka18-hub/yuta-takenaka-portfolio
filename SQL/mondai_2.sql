-- 商品を閲覧またはカートに追加したが、支払い記録がない顧客を取得する。
-- Retrieve customers who viewed or added products to their cart but have no payment records.

select c.cid, c.name
from customers as c
join add2cart as a on a.cid = c.cid
left join payment as p on p.cid = c.cid
where p.cid is null
union 
select c.cid, c.name
from customers as c
join viewedproduct as vp on vp.cid = c.cid
left join payment as p on p.cid = c.cid
where p.cid is null;