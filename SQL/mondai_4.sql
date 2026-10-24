-- カートに追加されたが、同じセッション内で購入されなかった商品と顧客を取得する。
-- Retrieve customers and products that were added to a cart but not purchased in the same session.

select distinct p.pid, c.cid, c.name
from customers as c
join add2cart as a on a.cid = c.cid
join products as p on p.pid = a.pid
where c.cid not in (
    select cid
    from orderSessions as os
    join orderlines as ol
    on ol.ono = os.ono
    and ol.pid = a.pid
    and os.cid = a.cid
    and os.sessionNo = a.sessionNo
);