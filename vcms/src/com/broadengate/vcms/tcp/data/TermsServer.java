package com.broadengate.vcms.tcp.data;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.Iterator;
import java.util.LinkedList;

import org.springframework.web.context.WebApplicationContext;

import com.broadengate.vcms.tcp.data.auxiliary.MessageManager;

public class TermsServer extends Thread {
	// private static int DEFAULT_SERVERPORT = 6325;//默认端口
	private static int DEFAULT_BUFFERSIZE = 1024;// 默认缓冲区大小为1024字节
	private static String DEFAULT_CHARSET = "GB2312";// 默认码集
	private static String DEFAULT_FILENAME = "bigfile.dat";
	private ServerSocketChannel channel;
	private LinkedList<SocketChannel> clients;
	private Selector selector;// 选择器
	private ByteBuffer buffer;// 字节缓冲区
	private int port;
	private Charset charset;// 字符集
	private CharsetDecoder decoder;// 解码器
	private WebApplicationContext webApp;// WebApplicationContext

	public TermsServer(int port, WebApplicationContext webApp)
			throws IOException {
		this.port = port;
		this.clients = new LinkedList<SocketChannel>();
		this.channel = null;
		this.selector = Selector.open();// 打开选择器
		this.buffer = ByteBuffer.allocate(DEFAULT_BUFFERSIZE);
		this.charset = Charset.forName(DEFAULT_CHARSET);
		this.decoder = this.charset.newDecoder();
		this.webApp = webApp;
	}

	protected void handleKey(SelectionKey key) {// 处理事件
	try{
		if (key.isAcceptable()) { // 接收请求
			ServerSocketChannel server = (ServerSocketChannel) key.channel();// 取出对应的服务器通道
			SocketChannel channel = server.accept();
			channel.configureBlocking(false);
			channel.register(selector, SelectionKey.OP_READ);// 客户socket通道注册读操作
		} else if (key.isReadable()) { // 读信息
			SocketChannel channel = (SocketChannel) key.channel();
			int count = channel.read(this.buffer);
			if (count > 0) {
				this.buffer.flip();
				byte[] commands = this.buffer.array();
				// 消息的后续处理
				MessageManager mm = new MessageManager();
				//得到消息的来源
				InetAddress ia = channel.socket().getInetAddress();
				int port = channel.socket().getPort();
				//
				boolean returnMark = mm.selector(commands, webApp,channel);
			} else {// 客户已经断开
				channel.close();
			}
			this.buffer.clear();// 清空缓冲区
		}
		}
		catch (IOException ioexc){
			System.out.println("有一个终端关闭了");
			key.cancel();
			if (key.channel()!=null) {
				try {
					key.channel().close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}

	public void run() {
		try {
			// 服务器开始监听端口，提供服务
			ServerSocket socket;
			channel = ServerSocketChannel.open(); // 打开通道
			socket = channel.socket(); // 得到与通到相关的socket对象
			socket.bind(new InetSocketAddress(port)); // 将scoket榜定在制定的端口上
			// 配置通到使用非阻塞模式，在非阻塞模式下，可以编写多道程序同时避免使用复杂的多线程
			channel.configureBlocking(false);
			channel.register(selector, SelectionKey.OP_ACCEPT);

			while (true) {// 与通常的程序不同，这里使用channel.accpet()接受客户端连接请求，而不是在socket对象上调用accept(),这里在调用accept()方法时如果通道配置为非阻塞模式,那么accept()方法立即返回null，并不阻塞
				this.selector.select();
				Iterator iter = this.selector.selectedKeys().iterator();
				while (iter.hasNext()) {
					SelectionKey key = (SelectionKey) iter.next();
					iter.remove();
					this.handleKey(key);
				}
			}
		} catch (IOException ex) {
			ex.printStackTrace();
		}

	}

	public void listen() throws IOException {
	}

	public static void main(String[] args) throws IOException {
		// System.out.println("服务器启动");
		// TermsServer server = new TermsServer(TermsServer.DEFAULT_SERVERPORT);
		// server.listen(); //服务器开始监听端口，提供服务
	}

}
