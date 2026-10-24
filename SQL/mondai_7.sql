-- 商品明細が4件以上あり、その半数以上が電子機器である注文を取得する。
-- Retrieve orders with more than three order lines, at least half of which are electronics.

select o.ono
from orders as o 
join orderlines as ol on ol.ono = o.ono
join products as p on p.pid = ol.pid
group by o.ono
having count(*) > 3
and sum(case when lower(p.category) = 'electronics' then 1 else 0 end) * 2
>= count(*);