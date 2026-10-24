-- 閲覧数、カート追加数、注文数から人気スコアを計算し、月ごとの上位3位までの商品を取得する。
-- Calculate product popularity scores based on views, cart additions, and orders, then retrieve the top-ranked products for each month.

with hotness as (
select ps.pid, ps.name, ps.yearMonth,
(ps.view_cnt + ps.add_cnt + 3 * ps.order_cnt) as score
from product_stats as ps),
rankings as (
select ht.pid, ht.name, ht.yearMonth, ht.score,
rank() over (
partition by ht.yearMonth
order by ht.score desc
) as ranks
from hotness as ht)

select yearMonth, pid, name, score
from rankings
where ranks <= 3
order by yearMonth asc, score desc;