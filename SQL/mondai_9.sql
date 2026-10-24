-- 過去12か月の商品別・月別の閲覧数、カート追加数、注文数を集計するビューを作成する。
-- Create a view that summarizes monthly product views, cart additions, and order counts over the past 12 months.

create view product_stats as

select p.pid, p.name, strftime('%Y-%m', o.odate) as yearMonth,
(select count(*) 
from viewedProduct as vp
where vp.pid = p.pid and strftime('%Y-%m', vp.ts) = strftime('%Y-%m', o.odate)) as view_cnt, 
(select count(*)
from add2cart as a
where a.pid = p.pid
and strftime('%Y-%m', a.ts) = strftime('%Y-%m', o.odate)) as add_cnt,
count(ol.ono) as order_cnt

from products as p
left join orderlines as ol on ol.pid = p.pid
left join orders as o on o.ono = ol.ono
where o.odate between datetime('now', '-12 month') and datetime('now')
group by p.pid, yearMonth;

select * from product_stats;