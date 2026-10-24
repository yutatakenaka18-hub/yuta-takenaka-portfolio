-- エドモントンに配送された電子機器を購入した顧客を取得する。
-- Retrieve customers who purchased electronics shipped to Edmonton.

select distinct c.cid, c.name, c.email
from customers as c
join orderSessions as os on os.cid = c.cid
join orders as o on o.ono = os.ono
join orderlines as ol on ol.ono = o.ono 
join products as p on p.pid = ol.pid
where p.category = 'electronics'
AND o.shipping_address like '%Edmonton%';