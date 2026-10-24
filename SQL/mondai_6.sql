-- 顧客ごとのカート追加からチェックアウトへの比率を計算する。
-- Calculate the cart-to-checkout conversion rate for each customer.

select c.cid, c.name,
(select cast(count(distinct co.sessionNo) as float) / cast(count(distinct a.sessionNo) as float)
from add2cart as a
left join checkout as co on co.cid = c.cid and a.sessionNo = co.sessionNo
where a.cid = c.cid) as conversion_rate
from customers as c;