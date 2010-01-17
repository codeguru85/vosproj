package dto.security.video
{
	import mx.collections.ArrayCollection;
	import mx.formatters.DateFormatter;	
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.VideoInfo")]
	public class VideoInfo
	{
		public var videoId:int;
		public var userInfo:UserInfo;
		public var videoName:String;
		public var createTime:Date;
		public var fileName:String;
		public var fileFormat:String;
		public var filePath:String;
		public var imageName:String;
		public var imagePath:String;
		public var fileSize:Number;
		public var timeCount:String;
		public var timeSecond:int;
		public var descn:String;
		public var setTime:String;
		public var keywords:ArrayCollection;
		public var videoTypes:ArrayCollection;
		public var videoAlbumShips:ArrayCollection;
		public var playListShips:ArrayCollection;
		public function VideoInfo()
		{
			//TODO: implement function
			//var dt:Date = new Date() ;
			//var fdate:DateFormatter = new DateFormatter() ;
			//fdate.formatString = "YYYY-MM-DD HH:NN:SS" ;
			//this.setTime = fdate.format(dt) ; 	
		}
	}
}