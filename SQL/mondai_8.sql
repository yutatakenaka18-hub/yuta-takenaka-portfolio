-- 顧客ごとのセッション数、平均セッション時間、平均活動回数、注文数を集計する。
-- Calculate the total sessions, average session duration, average activity count, and order count for each customer.

with session_information as (
    select s.cid, s.sessionNo,
    (julianday(s.end_time) - julianday(s.start_time)) * 1440 as len_session,
    count(a.ts) as num_activity
    from sessions as s
    left join activities a on s.cid = a.cid and s.sessionNo = a.sessionNo
    group by s.cid, s.sessionNo
)

select si.cid,
count(si.sessionNo) as sCnt,
cast(sum(si.len_session)/count(si.len_session) as int) as len,
round(avg(si.num_activity), 3) as meanCnt,
nullif(count(os.ono), 0) as orderCnt
from session_information as si
left join orderSessions as os on os.cid = si.cid and os.sessionNo = si.sessionNo
group by si.cid;