package com.taomee.statlogger;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * statlogger工具类
 * 
 * @author looper
 * @date 2016年5月13日
 */
public class StatCommon {

	public static String stat_trim_underscore(String str) {
		Integer cnt=0;
		//str.
		char[] cha=str.toCharArray();
		for(int i=0;i<cha.length;i++)
		{
			if(cha[i]=='_'||cha[i]==' ')
			{
				cnt++;
			}
			else
			{
				break;
			}
			//cnt++;
		}
		//for(int j=str.length();)
		int pos=str.length()-1;
		while(pos>0)
		{
			if(cha[pos]=='_'||cha[pos]==' ')
			{
				pos=pos-1;
			}
			else{
				break;
			}
		}
		//System.out.println(cnt);
		return str.substring(cnt, pos+1);
		/*for(int i=0;i<str.length();i++)
		{
			if(str.)
		}*/
	}

	public static String[] stat_split(String str, String delim) {
		return str.split(delim);
	}

	/*public static boolean stat_is_utf8(String strName) {
		try {
			Pattern p = Pattern.compile("\\s*|\t*|\r*|\n*");
			Matcher m = p.matcher(strName);
			String after = m.replaceAll("");
			String temp = after.replaceAll("\\p{P}", "");
			char[] ch = temp.trim().toCharArray();

			int length = (ch != null) ? ch.length : 0;
			for (int i = 0; i < length; i++) {
				char c = ch[i];
				if (!Character.isLetterOrDigit(c)) {
					String str = "" + ch[i];
					if (!str.matches("[\u4e00-\u9fa5]+")) {
						return true;
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		return false;
	}*/
	public static boolean isChinese(char c) {
        Character.UnicodeBlock ub = Character.UnicodeBlock.of(c);
        if (ub == Character.UnicodeBlock.CJK_UNIFIED_IDEOGRAPHS
                || ub == Character.UnicodeBlock.CJK_COMPATIBILITY_IDEOGRAPHS
                || ub == Character.UnicodeBlock.CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A
                || ub == Character.UnicodeBlock.GENERAL_PUNCTUATION
                || ub == Character.UnicodeBlock.CJK_SYMBOLS_AND_PUNCTUATION
                || ub == Character.UnicodeBlock.HALFWIDTH_AND_FULLWIDTH_FORMS) {
            return true;
        }
        return false;
    }
	
	public static boolean stat_is_utf8(String strName) {
		Pattern p = Pattern.compile("\\s*|t*|r*|n*");
        Matcher m = p.matcher(strName);
        String after = m.replaceAll("");
        String temp = after.replaceAll("\\p{P}", "");
        char[] ch = temp.trim().toCharArray();
        float chLength = ch.length;
        float count = 0;
        for (int i = 0; i < ch.length; i++) {
            char c = ch[i];
            if (!Character.isLetterOrDigit(c)) {
                if (!isChinese(c)) {
                    count = count + 1;
                }
            }
        }
        float result = count / chLength;
        if (result > 0.4) {
            return true;
        } else {
            return false;
        }
	}

	public static boolean size_between(String str, Integer min, Integer max) {
		Integer str_lenth = str.length();
		return (min < str_lenth && str_lenth < max);
	}

	public static boolean value_no_invalid_chars(String str) {
		return str.charAt(0) != '=' && str.charAt(0) != '!'
				&& str.charAt(0) != '\t';

	}

	public static boolean stat_makedir(String path) {
		String dir = "";
		Integer index = 0;
		if (path.length() == 0 || path.equals("")) {
			return false;
		}

		File dirs = new File(path);
		if (!dirs.exists()) {

			dirs.mkdirs();
			dirs.setExecutable(true);

		}

		// System.out.println(subdirs.length);
		return true;
	}
    /**
     * 判断操作系统是否为windows
     * @return
     */
	public static boolean isWindowsOS() {
		boolean isWindowsOS = false;
		String osName = System.getProperty("os.name");
		if (osName.toLowerCase().indexOf("windows") > -1) {
			isWindowsOS = true;
		}
		return isWindowsOS;
	}

	public static String stat_get_ip_addr() {
		String sIP = "";
		InetAddress ip = null;
		try {
			// 如果是Windows操作系统
			if (isWindowsOS()) {
				ip = InetAddress.getLocalHost();
			}
			// 如果是Linux操作系统
			else {
				boolean bFindIP = false;
				Enumeration<NetworkInterface> netInterfaces = (Enumeration<NetworkInterface>) NetworkInterface
						.getNetworkInterfaces();
				while (netInterfaces.hasMoreElements()) {
					if (bFindIP) {
						break;
					}
					NetworkInterface ni = (NetworkInterface) netInterfaces
							.nextElement();
					// ----------特定情况，可以考虑用ni.getName判断
					// 遍历所有ip
					Enumeration<InetAddress> ips = ni.getInetAddresses();
					while (ips.hasMoreElements()) {
						ip = (InetAddress) ips.nextElement();
						if (ip.isSiteLocalAddress() && !ip.isLoopbackAddress() // 127.开头的都是lookback地址
								&& ip.getHostAddress().indexOf(":") == -1) {
							bFindIP = true;
							break;
						}
					}

				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (null != ip) {
			sIP = ip.getHostAddress();
		}
		return sIP;
	}

}
