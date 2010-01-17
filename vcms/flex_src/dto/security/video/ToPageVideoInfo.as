package dto.security.video
{
	import mx.collections.ArrayCollection;
	
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.tcp.data.info.ToPageVideoInfo")]
	public class ToPageVideoInfo
	
	{		
		public var allTimes:String;
		public var nowTimes:String;
		public var nextName:String;
		public var videoId:int;
		public var videoName:String;
		public var createTime:Date;
		public var fileName:String;
		public var fileSize:int;
		public var timeCount:int;
		public var descn:String;
		public var videoTypes:ArrayCollection;
		public var stateMark:int ;

		public function ToPageVideoInfo()
		{
			//TODO: implement function
		}

	}
}