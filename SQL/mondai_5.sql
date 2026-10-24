-- カートに追加されたものの購入されなかった回数が多い上位5商品を取得する。
-- Identify the top five products with the highest number of cart abandonment occurrences.

select p.pid, p.name, count(*) as abandoned_count
from products as p
join add2cart as a on a.pid = p.pid
left join orderSessions as os on os.cid = a.cid and os.sessionNo = a.sessionNo
left join orderlines ol on os.ono = ol.ono and a.pid = ol.pid
where ol.ono is null
group by p.pid, p.name
order by abandoned_count desc limit 5;